import os, uuid

import numpy as np

from PIL import Image
from flask import Flask, request
app = Flask(__name__)

if not os.path.exists('img_store'):
    os.mkdir('img_store')

@app.route('/')
def hello_world():
    return 'Hello, World!'


@app.route('/upload_img', methods=['POST'])
def upload_img():
    if 'media' not in request.files:
        return 'Failure. Could not find media.', 400
    fname = 'img_store/' + str(uuid.uuid4()) + '.jpg'

    img = Image.open(request.files['media'])
    a = np.copy(np.asarray(img))
    print(a.shape)
    a[:,:,1] = 0 # remove green
    Image.fromarray(a).save(fname)
    return fname
