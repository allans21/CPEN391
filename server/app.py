import os, uuid

import numpy as np

from PIL import Image
from base64 import b64decode
from io import BytesIO
from flask import Flask, request
app = Flask(__name__)

if not os.path.exists('img_store'):
    os.mkdir('img_store')

@app.route('/')
def hello_world():
    return 'Hello, World!'


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


