-- This information is used by the Wi-Fi dongle to make a wireless connection to the router in the Lab
-- or if you are using another router e.g. at home, change ID and Password appropriately
-- SSID = "SM-G925W85885"
-- SSID_PASSWORD = "svcy3492"

-- I understand from students who have tried, that this also works, using UBC Visitor wireless connection
-- although I haven't tried it

-- SSID= "ubcvisitor"
-- SSID_PASSWORD = ""

-- configure ESP as a station
--wifi.setmode(wifi.STATION)
--wifi.sta.config(SSID,SSID_PASSWORD)
--wifi.sta.autoconnect(1)

-- alternatively you could do it this way

wifi.setmode(wifi.STATION)
wifi.sta.config("M112-PD","aiv4aith2Zie4Aeg", 1)
-- wifi.sta.config("TELUS5359-2.4G","kk523tfr4r", 1)
wifi.sta.autoconnect(1)
wifi.sta.connect()

-- pause for connection to take place - adjust time delay if necessary or repeat until connection made
tmr.delay(3000000) -- wait 1,000,000 us = 1 second

-- This should print 5 if connection was successful
-- print(wifi.sta.status())

-- Prints the IP given to ESP8266
-- print(wifi.sta.getip())

-- List all available wireless network ---
-- See documentation: https://nodemcu.readthedocs.io/en/master/en/modules/wifi/#wifistagetap

-- The following 2 pieces of information are related to your Twilio account the one you made in Exercise 1.9
-- change appropriately
TWILIO_ACCOUNT_SID = "AC394c741000dd79a82cf8611df4bc642e"
TWILIO_TOKEN =       "87a0c5ac353a10a48300c59741b2a45f"

-- Unfortunately, the Wi-FI dongle can only make unsecured HTTP requests, but Twilio requires 
-- secured HTTPS requests, so we will use a relay website to convert HTTP requests into HTTPS requests
-- visit http://iot-https-relay.appspot.com/ to learn more about this service
-- Please be sure to understand the security issues of using this relay app and use at your own risk.

-- this is the web address of the relay web site that our dongle sends the initial HTTP request to
HOST = "iot-https-relay.appspot.com" 

-- The following variable defines the TWILIO web site that we will connect to
-- use the first one if you want to send a text to a cell phone
-- use the second (commented out) one if you want to make a call to a cell phone - that's the only change
URI = "/twilio/Messages.json"
--URI = "/twilio/Calls.json"

function build_post_request(host, uri, data_table)

        data = ""

        for param,value in pairs(data_table) do
            data = data .. param.."="..value.."&"
        end

        request = "POST "..uri.." HTTP/1.1\r\n"..
        "Host: "..host.."\r\n"..
        "Connection: close\r\n"..
        "Content-Type: application/x-www-form-urlencoded\r\n"..
        "Content-Length: "..string.len(data).."\r\n"..
        "\r\n"..
        data
        print(request)
        return request
end

-- This function registers a function to echo back any response from the server, to our DE1/NIOS system 
-- or hyper-terminal (depending on what the dongle is connected to)
function display(sck,response)
        print(response)
end

-- When using send_sms: the "from" number HAS to be your twilio number.
-- If you have a free twilio account the "to" number HAS to be your twilio verified number.
function send_sms(from,to,body)

        data = {
        sid = TWILIO_ACCOUNT_SID,
        token = TWILIO_TOKEN,
        Body = string.gsub(body," ","+"),
        From = from,
        To = to
        }

        socket = net.createConnection(net.TCP,0)
        socket:on("receive",display)
        socket:connect(80,HOST)

        socket:on("connection",function(sck)
            post_request = build_post_request(HOST,URI,data)
            sck:send(post_request)
        end)
end

function check_wifi()
    ip = wifi.sta.getip()

    if(ip==nil) then
    print("Connecting...")
    else
    tmr.stop(0)
    print("Connected to AP!")
    print(ip)
    -- send a text message from, to, text
    send_sms("(204) 818-9230","(778) 866-2529","Ring-Ring - this is your breadboard calling!!!!")

    end
end

try = 1
gpio.write(3, gpio.LOW)
function try_connect() 
    if (try == 1) then
        wifi.sta.config("M112-PD","aiv4aith2Zie4Aeg", 1)
    elseif (try == 2) then
        wifi.sta.config("TELUS5359-2.4G","kk523tfr4r", 1)
    else
        connected = false
        gpio.write(3, gpio.LOW)
        print('failed')
    end
    try = try + 1
end

connected = nil
wifi.eventmon.register(wifi.eventmon.STA_DISCONNECTED, try_connect)
wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, function()
    connected = true
    gpio.write(3, gpio.HIGH)
    print('connected')
end)

local pin = 3            --  GPI0
local status = gpio.LOW
local duration = 200    -- 200 ms duration for timer

-- Initialising pin
gpio.mode(pin, gpio.OUTPUT)
gpio.write(pin, status)

-- Create an interval
tmr.alarm(1, duration, 1, function ()
    if (connected ~= nil) then
        tmr.unregister(1)
        return
    end
    if status == gpio.LOW then
        status = gpio.HIGH
    else
        status = gpio.LOW
    end

    gpio.write(pin, status)
end)
