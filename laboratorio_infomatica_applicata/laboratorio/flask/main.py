from flask import Flask, session

app = Flask(__name__)
app.secret_key = "a"

def HelloWorld():
    return "Hello World from Flask"

@app.route('/') 
def displaySessionVisits():
    session['visits'] = session.get("visits", 0) + 1;
    return f"Hai visitato la pagina {session['visits']} volte"

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000, debug=True)
