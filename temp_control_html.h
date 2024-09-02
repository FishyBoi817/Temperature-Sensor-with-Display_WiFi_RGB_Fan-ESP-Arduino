#ifndef ESP_HTML_H
#define ESP_HTML_H

// Your HTML content or declarations go here
const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Control</title>
    <style>
        body {
            font-family: Verdana, Arial, sans-serif;
            background-color: #efefef;
        }
        .navbar {
            width: 100%;
            height: 50px;
            background-color: #FFF;
            border-bottom: 5px solid #293578;
            padding: 10px 0;
            text-align: center;
            font-size: 24px;
            font-weight: bold;
        }
        .container {
            max-width: 600px;
            height: auto;
            margin: 20px auto;
            text-align: center;
            background-color: lightgrey;
            border-radius: 10px;
            border: 2px solid #333;
            box-shadow: 4px 4px 8px rgba(0, 0, 0, 0.3);
            padding: 20px;
        }
        .btn {
            background-color: #444444;
            border: none;
            color: white;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
        }
        .foot {
            font-size: 20px;
            text-align: center;
            color: #AAAAAA;
            padding: 10px 0;
        }
        .led-container {
            margin: 20px auto;
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
        }
        .led {
            width: 100px;
            height: 100px;
            border-radius: 50%;
            background-color: #FFFFFF; 
            box-shadow: 0 0 15px rgba(0, 0, 0, 0.5); 
            position: relative;
            transition: background-color 0.5s, box-shadow 0.5s; 
        }
        .led:before {
            content: '';
            position: absolute;
            width: 100%;
            height: 100%;
            border-radius: 50%;
            top: 0;
            left: 0;
            background: radial-gradient(circle at center, rgba(255, 255, 255, 0.4) 0%, rgba(0, 0, 0, 0) 60%);
            transition: opacity 0.5s;
        }
        .led-on:before {
            opacity: 1;
        }
        .led-off:before {
            opacity: 0;
        }
        .toggle {
            position: relative;
            display: inline-block;
            width: 100px;
            height: 52px;
            background-color: red;
            border-radius: 30px;
            border: 2px solid gray;
        }
        .toggle:after {
            content: '';
            position: absolute;
            width: 50px;
            height: 50px;
            border-radius: 50%;
            background-color: gray;
            top: 1px;
            left: 1px;
            transition: all 0.5s;
        }
        p {
            font-family: Verdana, Arial, sans-serif;
            font-weight: bold;
            color: white;
        }
        .checkbox:checked + .toggle::after {
            left: 49px;
        }
        .checkbox:checked + .toggle {
            background-color: green;
        }
        .checkbox {
            display: none;
        }
        .toggleButton {
            padding-top: 40px;
            padding-bottom: 40px;
        }
        .color-controls {
            margin-top: 20px;
        }
        .color-controls input {
            width: 60px;
            margin: 5px;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="navbar">ESP32 Control</div>
    <div class="container">
        <h2>Control LED</h2>
        <div class="led-container">
            <div id="led" class="led led-off"></div>
            <div class="toggleButton">
                <center>
                    <input type="checkbox" id="switch" class="checkbox" onclick="sendToggleState()" />
                    <label for="switch" class="toggle">
                        <p>ON &nbsp;&nbsp; OFF</p>
                    </label>
                </center>
            </div>
            <div class="color-controls">
                <h3>Set RGB</h3>
                <input type="number" id="red" placeholder="R" min="0" max="255">
                <input type="number" id="green" placeholder="G" min="0" max="255">
                <input type="number" id="blue" placeholder="B" min="0" max="255">
                <button class="btn" onclick="setColor()">Set Color</button>
            </div>
        </div>
        <footer class="foot">ESP32 Web Page</footer>
    </div>
    <script type="text/javascript">
        function sendToggleState() {
            var switchElement = document.getElementById('switch');
            var ledElement = document.getElementById('led');
            var state = switchElement.checked ? 'ON' : 'OFF';
            console.log('Current toggle state:', state);

            if (switchElement.checked) {
                ledElement.classList.remove('led-off');
                ledElement.classList.add('led-on');
            } else {
                ledElement.classList.remove('led-on');
                ledElement.classList.add('led-off');
            }

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "/BUTTON_" + state, true);
            xhttp.send();
        }

        function setColor() {
            var red = parseInt(document.getElementById('red').value, 10);
            var green = parseInt(document.getElementById('green').value, 10);
            var blue = parseInt(document.getElementById('blue').value, 10);

          
            if (isNaN(red) || red < 0 || red > 255) {
                red = 0;
            }
            if (isNaN(green) || green < 0 || green > 255) {
                green = 0;
            }
            if (isNaN(blue) || blue < 0 || blue > 255) {
                blue = 0;
            }

            var color = 'rgb(' + red + ',' + green + ',' + blue + ')';
            var ledElement = document.getElementById('led');

            ledElement.style.backgroundColor = color;
            ledElement.style.boxShadow = '0 0 20px ' + color;

           
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "/COLOR?R=" + red + "&G=" + green + "&B=" + blue, true);
            xhttp.send();
        }
    </script>
</body>
</html>

)=====";

#endif // ESP_HTML_H