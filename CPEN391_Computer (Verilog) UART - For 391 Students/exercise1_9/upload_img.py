import requests

url = 'http://localhost:5000/upload_img'
files = {'media': open('person.jpg', 'rb')}
res = requests.post(url, files=files)

print('\n'.join(res.text.split('<br/>')))