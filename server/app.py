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
import db.products
import datetime
import time
app = Flask(__name__)

def user_id2access_token(user_id):
    return str(user_id)

def access_token2user_id(access_token):
    return int(access_token)

def getuser():
    user = None;
    access_token = request.cookies.get('access_token')
    if access_token:
        if(access_token == "40"):
            user = None
        else:        
            db_conn = db.utils.get_connection()
            customer = db.customer.getCustomerByID(db_conn, access_token)
            user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    else:
        access_token = 40
        user = None
    return access_token, user


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
    access_token, user = getuser()
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
    access_token, user = getuser()
    return render_template('Group5.html', user = user)

@app.route('/HomeScreen.html')
def homescreen():
    access_token, user = getuser()
    return render_template('HomeScreen.html', user = user)


@app.route('/CustomersAccount.html')
def custaccount():
    access_token, user = getuser()
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
        return render_template("ErrorMessage.html", error_message="Missing username or password", new_page="HomeScreen.html")

    db_conn = db.utils.get_connection()
    customer = db.customer.getCustomerByEmail(db_conn, email)
    if not customer:
        return render_template("ErrorMessage.html", error_message="Wrong username or password", new_page="HomeScreen.html")
        
    access_token = customer.id 
    if(customer.password == userpass):
        user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
        return render_template('SignIn.html', access_token=access_token, user=user)
    else:
        return render_template("ErrorMessage.html", error_message="Wrong username or password", new_page="HomeScreen.html")

@app.route('/signout', methods=['POST'])
def signout():
    return render_template('signout.html', access_token=40, user=None)

@app.route('/signup', methods=['POST'])
def signup():
    email, password, name, phone, address, card, liID = request.form.get('newemail'), request.form.get('newpassword'), request.form.get('newName'), request.form.get('newphone'), request.form.get('newAddress'), request.form.get('newCard'), request.form.get('newID')

    if not email or not password or not name or not phone or not address or not card or not liID:
        return render_template("ErrorMessage.html", error_message="Missing fields, please fill out all information and resubmit", new_page="SignUp.html")

    today = datetime.date.today()
    db_conn = db.utils.get_connection()
    date = db.customer.getDate(db_conn, liID)
    age = abs((today - date).days)
    if(age < 6935):
        return render_template("ErrorMessage.html", error_message="Sorry you are not of legal age and can not make an account until you are 19", new_page="HomeScreen.html")

    db.customer.insertCustomer(db_conn, liID, email, address, phone, name, 0, password, card)

    customer = db.customer.getCustomerByEmail(db_conn, email)
    access_token = customer.id
    print(customer.password)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('signingup.html', access_token=access_token, user=user)

@app.route('/update-name', methods=['post'])
def Namechange():
    name = request.form.get('updname')
    if not name :
        return render_template("ErrorMessage.html", error_message="Missing new name", new_page="CustomersAccount.html")


    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changeName(db_conn, name, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-email', methods=['post'])
def emailchange():
    email = request.form.get('updemail')
    if not email :
        return render_template("ErrorMessage.html", error_message="Missing new email", new_page="CustomersAccount.html")

    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changeEmail(db_conn, email, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-address', methods=['post'])
def addresschange():
    address = request.form.get('updaddress')
    if not address :
        return render_template("ErrorMessage.html", error_message="Missing new address", new_page="CustomersAccount.html")

    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changeAddress(db_conn, address, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-dlID', methods=['post'])
def dlIDchange():
    dlID = request.form.get('upddlID')
    if not dlID :
        return render_template("ErrorMessage.html", error_message="Missing new license ID", new_page="CustomersAccount.html")

    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changeDL(db_conn, dlID, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-phone', methods=['post'])
def phonechange():
    phone = request.form.get('updphone')
    if not phone :
        return render_template("ErrorMessage.html", error_message="Missing new phone number", new_page="CustomersAccount.html")

    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changePhone(db_conn, phone, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-pass', methods=['post'])
def passchange():
    passw = request.form.get('updpass')
    if not passw :
        return render_template("ErrorMessage.html", error_message="Missing new password", new_page="CustomersAccount.html")

    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changePass(db_conn, passw, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-card', methods=['post'])
def cardchange():
    card = request.form.get('updcard')
    if not card :
        return render_template("ErrorMessage.html", error_message="Missing new Credit Card number", new_page="CustomersAccount.html")

    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    db.customer.changeCard(db_conn, card, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

@app.route('/update-credits', methods=['post'])
def creditschange():
    credits = request.form.get('updcredits')
    if not credits :
        return render_template("ErrorMessage.html", error_message="Missing how many credits to add", new_page="CustomersAccount.html")

    if isinstance(credits, str):
        return render_template("ErrorMessage.html", error_message="You did not enter a number for how many credits to add", new_page="CustomersAccount.html")


    access_token = request.cookies.get('access_token')    
    db_conn = db.utils.get_connection()
    customer = db.customer.getCustomerByID(db_conn, access_token)

    credits = int(credits) + int(customer.credits)
    db.customer.changeCredits(db_conn, credits, access_token)

    customer = db.customer.getCustomerByID(db_conn, access_token)
    user = {'name': customer.name, 'credits': customer.credits, 'id': customer.id, 'email': customer.email, 'address': customer.address, "dlID": customer.dl_id, "cardNum": customer.cardnumber, "phone": customer.phone_number, "password":customer.password}
    return render_template('CustomersAccount.html', access_token=access_token, user=user)

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
    
    if 'item_ids' not in request.form:
        return 'No itemIds', 400
    if 'machine_id' not in request.form:
        return ' No machinesId', 400

    user_id = request.form['user_id']
    item_ids = request.form.getlist('item_ids')
    machine_id = request.form['machine_id']
    db_conn = db.utils.get_connection()

    remainingCredits= db.customer.getCredits(db_conn, user_id)
   
    totalCredits=0
    for stock_id in item_ids:
        print('Getting stock #', stock_id)
        if not db.stock.getStockById(db_conn,stock_id):
            return 'not enough stock', 400
        totalCredits = totalCredits + db.products.getPriceByStock(db_conn,stock_id)
        

   
    
    if(totalCredits > remainingCredits):
        return 'Too Many Items', 400
    else:
        newCredit = remainingCredits - totalCredits
        
        for item_id in item_ids:
            db.stock.updateStock(db_conn,item_id, machine_id)
        db.customer.updateCredits(db_conn, user_id, newCredit)
        
    data = {}
    data['newCredit'] = newCredit
    return json.dumps(data)


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)

