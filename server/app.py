import os, uuid, json

import numpy as np

from PIL import Image
from base64 import b64decode
from io import BytesIO
from flask import Flask, request, send_from_directory, make_response, render_template
from werkzeug.routing import BaseConverter

import db.utils
import db.stock
import db.customer
app = Flask(__name__)

def user_id2access_token(user_id):
    return str(user_id)

def access_token2user_id(access_token):
    return int(access_token)


@app.after_request
def add_header(r):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """
    r.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    r.headers["Pragma"] = "no-cache"
    r.headers["Expires"] = "0"
    r.headers['Cache-Control'] = 'public, max-age=0'
    return r

@app.route('/')
def root():
    return render_template('HomeScreen.html')

if not os.path.exists('img_store'):
    os.mkdir('img_store')


class RegexConverter(BaseConverter):
    def __init__(self, url_map, *items):
        super(RegexConverter, self).__init__(url_map)
        self.regex = items[0]

app.url_map.converters['regex'] = RegexConverter

@app.route('/CustomersAccount.html')
def custaccount():
    access_token = request.cookies.get('access_token')
    if access_token:
        user_id = access_token2user_id(access_token)
        # TODO get user from database by ID, return error if no user
        user = {'name': 'Aidan Rosswood', 'credits': 10000, 'id': 500}
    else:
        user = None

    return render_template('CustomersAccount.html', user=user)


@app.route('/<regex(".+\\.(html|js|css)$"):fname>')
def serve_html(fname):
    access_token = request.cookies.get('access_token')
    return render_template(fname, access_token=access_token)


@app.route('/photos/<regex(".+\\.(jpg|JPG|jpeg|JPEG)$"):fname>')
def serveimg(fname):
    image_binary = open(os.path.join('templates/photos', fname), 'rb').read()
    response = make_response(image_binary)
    response.headers.set('Content-Type', 'image/jpeg')
    response.headers.set(
        'Content-Disposition', 'attachment', filename=fname)
    return response


@app.route('/signin', methods=['POST'])
def signin():
    email, userpass = request.form.get('email'), request.form.get('userpass')
    if not email or not userpass:
        return "Missing email or password", 400 # return error

    # TODO actually get user from database or return error if they arent in the DB, get their access token also
    access_token = "420"

    return render_template('SignIn.html', access_token=access_token)


@app.route('/user', methods=['GET'])
def get_user():
    if 'barcode_id' not in request.form:
        return 'No barcode_id', 400

    barcode_id = request.form['barcode_id']
    db_conn = db.utils.get_connection()
    customer = db.customer.getCustomerByBarcode(db_conn, barcode_id)

    if not customer:
        return 'No customer for that barcode', 400

    return json.dumps({
        'ID': customer.id,
        'NAME': customer.name, 
        'PHONENUMBER': customer.phone_number,
        'CREDITS': customer.credits
    })

@app.route('/inventory', methods=['GET'])
def get_inventory():
    if 'machineId' not in request.form:
        return 'No machineId', 400

    machine_id = request.form['machineId']
    db_conn = db.utils.get_connection()
    inventory = db.stock.getInventory(db_conn, machine_id)

    if inventory is None:
        return 'Bad machineId', 400

    return json.dumps(inventory)

