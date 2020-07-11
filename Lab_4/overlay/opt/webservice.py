from flask import Flask, flash, request, redirect, url_for, session, escape, send_from_directory
from werkzeug.utils import secure_filename
from mpd import MPDClient
import os
import threading

ALLOWED_EXTENSIONS = {'mp3'}

app = Flask(__name__)

userList=dict()
userList["admin"]="password"

app.secret_key = 'LINES'
path = '../var/lib/mpd/music/'

client = MPDClient()
client_lock = threading.Lock()
with client_lock:
    client.connect("localhost", 6600)
    client.update()

def bound(value, low, high):
    if value < low:
        value = low
    if value > high:
        value = high
    return value

def reconnect():
    try:
        client.ping()
    except Exception:
        client.connect("localhost", 6600)
    finally:
	    client.update()

@app.route("/")
def hello():
    reconnect()
    entries = client.playlist()
    output=""
    i = 1
    for entry in entries:
        output+=str(i)+" "
        i+=1
        p = entry.split(' ')[1]
        output+= p
        if(output!=""):
            output+="<br>"
    output += """
    <form>
    <input type=text name=file>
    <input type=submit value=Up formaction="/up">
    <input type=submit value=Down formaction="/down">
    </form>
    """
    return output

@app.route("/up")
def up():
    reconnect()
    filename = request.args.get('file')
    if filename in os.listdir(path):
        playlist = client.playlist()
        maxp = len(playlist) -1
        i = 0
        while i < len(playlist):
            p = playlist[i].split(' ')[1]
            if p == filename:
                break
            i+=1
        i = bound(i,0,maxp)
        if i > 0 :
            client.swap(i, i-1)
    return redirect(url_for("hello"))

@app.route("/down")
def down():
    reconnect()
    filename = request.args.get('file')
    if filename in os.listdir(path):
        playlist = client.playlist()
        maxp = len(playlist) - 1
        i = 0
        while i < len(playlist):
            p = playlist[i].split(' ')[1]
            if p == filename:
                break
            i+=1
        i = bound(i,0,maxp)
        if i < maxp :
            client.swap(i, i+1)
    return redirect(url_for("hello"))

def allowed_file(filename):
    f = filename.split('.', 1)
    if len(f) != 2:
        return false
    return filename.split('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route('/upload', methods=['GET', 'POST'])
def upload_file():
    reconnect()
    if 'username' in session:
        if request.method == 'POST':
            if 'file' not in request.files:
                flash('No file part')
                return redirect(request.url)
            file = request.files['file']
            if file.filename == '':
                flash('No selected file')
                return redirect(request.url)
            if file and allowed_file(file.filename):
                filename = secure_filename(file.filename)
                file.save(os.path.join(path, filename))
                os.system('mpc update --wait')
                os.system('mpc add ' + filename)
                return "successed"
        return '''
        <!doctype html>
        <title>Upload new File</title>
        <h1>Upload new File</h1>
        <form method=post enctype=multipart/form-data>
        <input type=file name=file>
        <input type=submit value=Upload>
        </form>
        '''
    return 'Firstly, log in by "/login"'

@app.route("/downloaded")
def downloaded():
    reconnect()
    filename = request.args.get('file')
    if filename in os.listdir(path):
        return send_from_directory(path, filename, as_attachment=True)
    else:
        return "No such file on server"

@app.route("/download")
def download():
    return '''
        <form action="/downloaded">
        <input type=text name=file>
        <input type=submit value=Download>
        </form>
        '''

@app.route('/login', methods=['GET', 'POST'])
def login():
    if 'username' in session:
        return 'You are logged in'
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if userList[username] == password :
            session['username'] = request.form['username']
            return redirect(url_for('login'))
        else:
            return 'Invalid username/password'
    return '''
        <form action="" method="post">
            <p><input type=text name=username>
            <p><input type=password name=password>
            <p><input type=submit value=Login>
        </form>
        '''

@app.route('/logout')
def logout():
    session.pop('username', None)
    return redirect(url_for('login'))

if __name__ == '__main__':
    app.run(debug=True, port=80, host='0.0.0.0')
