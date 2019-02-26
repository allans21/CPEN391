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
    user = None;
    access_token = request.cookies.get('access_token')
    if access_token:
        if(access_token == "40"):
            user = None
        else:        
            db_conn = db.utils.get_connection()
            customer = db.customer.getCustomerByID(db_conn, access_token)
            user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id}
    else:
        user = None

    return render_template('HomeScreen.html', user = user)

if not os.path.exists('img_store'):
    os.mkdir('img_store')


class RegexConverter(BaseConverter):
    def __init__(self, url_map, *items):
        super(RegexConverter, self).__init__(url_map)
        self.regex = items[0]

app.url_map.converters['regex'] = RegexConverter

@app.route('/Group5.html')
def group():
    user = None;
    access_token = request.cookies.get('access_token')
    if access_token:
        if(access_token == "40"):
            user = None
        else:        
            db_conn = db.utils.get_connection()
            customer = db.customer.getCustomerByID(db_conn, access_token)
            user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id}
    else:
        user = None
    return render_template('Group5.html', user = user)

@app.route('/HomeScreen.html')
def homescreen():
    user = None;
    access_token = request.cookies.get('access_token')
    if access_token:
        if(access_token == "40"):
            user = None
        else:        
            db_conn = db.utils.get_connection()
            customer = db.customer.getCustomerByID(db_conn, access_token)
            user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id}
    else:
        user = None
    return render_template('HomeScreen.html', user = user)


@app.route('/CustomersAccount.html')
def custaccount():
    access_token = request.cookies.get('access_token')
    if access_token:
        db_conn = db.utils.get_connection()
        customer = db.customer.getCustomerByID(db_conn, access_token)
        print(customer.name)
        user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id}
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
    token = request.cookies.get('access_token')
    print("hither")
    print(token)
    if(token == '40'):
        print("inhere")
        if not email or not userpass:
            return "Missing email or password", 400 # return error

        db_conn = db.utils.get_connection()
        customer = db.customer.getCustomerByEmail(db_conn, email)
        access_token = customer.id
        print(customer.password)
        if(customer.password == userpass):
            user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id}
            return render_template('SignIn.html', access_token=access_token, user=user)
        else:
            return "Wrong Username or Password", 400 # return error
    else:
        user = None
        return render_template('SignIn.html', access_token=40, user=user)

@app.route('/signup', methods=['POST'])
def signup():
    email, password, name, phone, address, card, liID = request.form.get('newemail'), request.form.get('newpassword'), request.form.get('newName'), request.form.get('newphone'), request.form.get('newAddress'), request.form.get('newCard'), request.form.get('newID')

    print(email)
    print(password)
    print(name)
    if not email or not password or not name or not phone or not address or not card or not liID:
        return "some fields are missing", 400 #return error
    print("MADE IT ######################################################################")

    db_conn = db.utils.get_connection()
    db.customer.insertCustomer(db_conn, liID, email, address, phone, name, 0, password, card)
    print("here")
    
    customer = db.customer.getCustomerByEmail(db_conn, email)
    access_token = customer.id
    print(customer.password)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id}
    return render_template('signingup.html', access_token=access_token, user=user)


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

@app.route('/purchase', methods=['PUT'])
def put_purchase():
    if 'user_id' not in request.form:
        return 'No userId', 400
    
    if 'item_id' not in request.form:
        return 'No itemId', 4000

    user_id = request.form['user_id']
    item_id = request.form['item_id']
    db_conn = db.utils.get_connection()
    return json.dumps([])
