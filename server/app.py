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

@app.route('/<regex(".+\\.(html|js|css)$"):fname>')
def serve_html(fname):
    return render_template(fname)


@app.route('/photos/<regex(".+\\.(jpg|JPG|jpeg|JPEG)$"):fname>')
def serveimg(fname):
    image_binary = open(os.path.join('templates/photos', fname), 'rb').read()
    response = make_response(image_binary)
    response.headers.set('Content-Type', 'image/jpeg')
    response.headers.set(
        'Content-Disposition', 'attachment', filename=fname)
    return response


@app.route('/upload_img', methods=['POST'])
def upload_img():
    print("request.data", request.data)
    print("request.form", request.form)
    if 'img_id' not in request.form:
        return 'Failure. Could not find media.', 400
    img_id = str(int(request.form['img_id'].strip('&')))
    chunk_num = 0
    fname = 'img_store/' + img_id  + '_0.b64'
    b64_img = []
    while os.path.exists(fname):
        b64_img.append(open(fname).read())
        chunk_num += 1
        os.remove(fname)
        fname = 'img_store/' + img_id + '_' + str(chunk_num) + '.b64'
    

    b64_img = ''.join(b64_img)
    if len(b64_img) % 4 != 0:
        b64_img = b64_img + ('=' * (4-(len(b64_img)%4)))
    print(b64_img)
    img_bytes = b64decode(b64_img)
    img = Image.open(BytesIO(img_bytes))
    img.save('img_store/' + img_id + '.jpg')
    return fname

@app.route('/upload_chunk', methods=['POST'])
def upload_chunk():
    if 'img_id' not in request.form:
        return 'No img_id', 400
    if 'chunk' not in request.form:
        return 'No chunk', 400
    if 'chunk_num' not in request.form:
        return 'No chunk_num', 400
     
    chunk_num = str(int(request.form['chunk_num'].strip('&')))
    img_id = str(int(request.form['img_id'].strip('&')))
    chunk = request.form['chunk'].replace(' ', '+').strip('&').strip() # Flask decodes the +'s to space. We need the +'s as they are for B64
    print('%s #%s == %s' % (img_id, chunk_num, chunk))
    fname = 'img_store/' + img_id + '_' + chunk_num + '.b64'
    with open(fname, 'a+') as f:
        f.write(chunk)
    
    return 'OK', 200

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

