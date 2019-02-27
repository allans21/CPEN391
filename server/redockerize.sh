echo 'PULLING'
git pull
echo 'BUILDING'
docker build . -t vending
echo 'RUNNING AS DAEMON'
docker run -d -p 5000:5000 vending

