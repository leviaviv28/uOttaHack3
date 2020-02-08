import flask

app = flask.Flask(__name__)
app.config["DEBUG"] = True


@app.route('/update_location', methods=['POST'])
def update():
    return "<p>Received</p>"

app.run()