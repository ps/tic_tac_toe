import logging
import os
import subprocess
from datetime import date
from flask import Flask,render_template,request
from logging.handlers import RotatingFileHandler

app=Flask(__name__)

@app.route('/')
def main_page():
    # will display main game page
    year = date.today().year
    return render_template('index.html', year=year)

@app.route('/api')
def api_docs():
    # serves api documentation page
    return render_template('api_doc.html')

@app.route('/get_winner', methods = ['GET', 'POST'])
def winner_request():
    # getting the winner api endpoint
    if request.method == 'GET' and 'board' in request.args:
        return exec_ai(request.args["board"], 'o', '-w')
    elif request.method == 'POST' and "board" in request.form:
        return exec_ai(request.form["board"], 'o', '-w')
    return render_template("data_error.html")

@app.route('/get_move', methods = ['GET', 'POST'])
def move_request():
    # getting next move api endpoint
    if (request.method == 'GET' and 'board' in request.args 
            and 'player' in request.args):
        return exec_ai(request.args["board"], request.args["player"], "-m")
    elif (request.method == 'POST' and "board" in request.form 
            and "player" in request.form):
        return exec_ai(request.form["board"], request.form["player"], "-m")
    return render_template("data_error.html")

def exec_ai(board_data, player, flag):
    abspath_cwd = os.path.dirname(os.path.abspath(__file__))
    script_path = os.path.join(abspath_cwd, 'backend_ai/./TTAI')
    return subprocess.check_output([script_path, board_data, player, flag])

if __name__ == "__main__":
    abspath_cwd = os.path.dirname(os.path.abspath(__file__))
    file_handler = RotatingFileHandler(os.path.join(abspath_cwd, '../tictac_error.log'), 
        maxBytes=1024 * 1024 * 100, backupCount=20)
    file_handler.setLevel(logging.INFO)
    formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    file_handler.setFormatter(formatter)
    app.logger.addHandler(file_handler)
    app.run(debug=True)
