import flask
import json

app = flask.Flask(__name__)
app.config["DEBUG"] = True

packages = {}
routers = {}


@app.route('/update_location', methods=['POST'])
def update_location():
    tracking = flask.request.form['tracking']
    node = flask.request.form['node']

    packages[tracking] = routers[node]
    
    return "Received"


@app.route('/get_location/<int:tracking>', methods=['GET'])
def get_location(tracking):   
    return json.dumps(packages[tracking])


app.run()