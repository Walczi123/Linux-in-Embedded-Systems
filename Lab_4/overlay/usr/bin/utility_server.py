from flask import Flask, flash, request, redirect, url_for, session, escape, send_file
from werkzeug.utils import secure_filename
from mpd import MPDClient
import os

ALLOWED_EXTENSIONS = {'mp3'}

app = Flask(__name__)

userList=dict()
userList["admin"]="password"

app.secret_key = 'LINES'
path = '/var/lib/mpd/music'

client = MPDClient()
client.connect("localhost", 6600)
client.update()
client.add('./')
client.play()
client.close()
client.disconnect()

@app.route("/")
def hello():
    entries = os.listdir(path)
    output=""
    for entry in entries:
        if(output!=""):
            output+=", "
        output+= entry
    return output

def allowed_file(filename):
    return path in filename and \
           filename.rsplit(path, 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route('/upload', methods=['GET', 'POST'])
def upload_file():
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
                client.add(filename)
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
   
@app.route('/download', methods=['GET', 'POST'])
def download(filename):
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
                return send_file(filename, as_attachment=True)
        return '''
        <!doctype html>
        <title>Download File</title>
        <form method=post enctype=multipart/form-data>
        <input type=file name=file>
        <input type=submit value=Upload>
        </form>
        '''

@app.route('/login', methods=['GET', 'POST'])
def login():
    if 'username' in session:
        return 'You are not logged in'
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