#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>        // Include WiFi library
#include <ArduinoJSON.h>// for get requests

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#include <Fonts/FreeMonoBold18pt7b.h>
#include <Adafruit_NeoPixel.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#include <Preferences.h>

#define ENABLE_GxEPD2_GFX 0
#include <map>

std::map<String, StockPriceStruc> stockMap;

struct StockPriceStruc {
  float currentPrice;
  float percentageChange;
  float openPrice;
  float highPrice;
  float lowPrice;
  float closePrice;
  String newstitle;
  String newssource;
};
// A0OFEISI3OBSH41P api key alpha vantage

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ 5, /*DC=*/ 4, /*RES=*/ 2, /*BUSY=*/ 15)); // GDEY0213Z98 122x250, SSD1680
WiFiClient client;
int Setupwifi();
int GetStockYahooFinanceHttps(String symbol);
int helloWorld(String symbol,float currentPrice,float percentageChange);
String jsonBuffer;
struct StockPriceStruc stockprice;
Adafruit_NeoPixel pixels(2, 13, NEO_GRB + NEO_KHZ800);
int setpixelcolourstock();
int GetLatestPackageStatus(String trackingid);
int GetWeatherData(float longitide,float latitude);
int GetStockNews(String symbol);
int displayStockNews(String symbol,String title,String source);
void saveArrayWithUUID( std::vector<String> data);
std::vector<String> loadArrayWithUUID(const String& uuid);
String processor(const String& var);
void addUUID(String newUUID);
void updateArrayWithUUID(std::vector<String> data, const String& uuid);
void displayiteminsequence();
AsyncWebServer ayncserver(80);
Preferences preferences;
const char* serverparamcategory = "category";
const char* serverparamstockname = "stockname";
const char* serverparamlattitude = "lattitude";
const char* serverparamlongitude = "longitude";
const char* serverparamuid = "uid";

String generateUID();


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>StokTake</title>

  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&family=Roboto+Slab:wght@100..900&display=swap" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/sortablejs@latest/Sortable.min.js"></script>
  <style>
    body {
      margin: 10px;
      background-color: #D9D9D9;
      text-align: center;
      font-family: "Roboto Slab", "Inter", sans-serif;
      font-size: 24px;
    }

    .glyphicon-move {
      cursor: move;
      cursor: -webkit-grabbing;
    }
    .home {
      font-weight: 300;
      font-size: 64px;
    }
    .divbox {
      background-color: #266788;
      border-radius: 10px;
      margin: 20px auto;
      width: 20%%;
      box-shadow: 10px 10px 20px rgba(0, 0, 0, 0.25);
      border-style: solid;
      border-width: 4px;
    }
    .divboxtext {
      color: #FFFFFF;

    }
    .addnewitemdiv{
      background-color: #B8B8B8;
      border-radius: 10px;
      margin: 20px auto;
      width: 20%%;
      box-shadow: 10px 10px 20px rgba(0, 0, 0, 0.25);
      border-style: solid;
      border-width: 4px;
      color: black;
    }
    .addnewitemdiv>p{

      color: black;
    }

        /* The Modal (background) */
    .modal {
      display: none; /* Hidden by default */
      position: fixed; /* Stay in place */
      z-index: 1; /* Sit on top */
      padding-top: 100px; /* Location of the box */
      left: 0;
      top: 0;
      width: 100%%; /* Full width */
      height: 100%%; /* Full height */
      overflow: auto; /* Enable scroll if needed */
      background-color: rgb(0,0,0); /* Fallback color */
      background-color: rgba(0,0,0,0.4); /* Black w/ opacity */
    }

    /* Modal Content */
    .modal-content {
      background-color: #fefefe;
      margin: auto;
      padding: 20px;
      border: 1px solid #888;
      width: 80%%;
    }

    /* The Close Button */
    .close {
      color: #aaaaaa;
      float: right;
      font-size: 28px;
      font-weight: bold;
    }

    .close:hover,
    .close:focus {
      color: #000;
      text-decoration: none;
      cursor: pointer;
    }
     /* The container must be positioned relative: */
    .custom-select ,.custom-selectedit{
      position: relative;
      font-family: Arial;
      margin: 20px auto;
      background-color: #266788;
      border-radius: 10px;
      margin: 20px auto;
      width: 20%%;
      box-shadow: 10px 10px 20px rgba(0, 0, 0, 0.25);
      border-style: solid;
      border-width: 4px;
    }

    .custom-select select ,.custom-selectedit select{
      display: none; /*hide original SELECT element: */
    }

    .select-selected {
      background-color: #266788;
      border-radius: 10px;
    }

    /* Style the arrow inside the select element: */
    .select-selected:after {
      position: absolute;
      content: "";
      top: 14px;
      right: 10px;
      width: 0;
      height: 0;
      border: 6px solid transparent;
      border-color: #fff transparent transparent transparent;
    }

    /* Point the arrow upwards when the select box is open (active): */
    .select-selected.select-arrow-active:after {
      border-color: transparent transparent #fff transparent;
      top: 7px;
    }

    /* style the items (options), including the selected item: */
    .select-items div,.select-selected {
      color: #ffffff;
      padding: 8px 16px;
      border: 1px solid transparent;
      border-color: transparent transparent rgba(0, 0, 0, 0.1) transparent;
      cursor: pointer;
    }

    /* Style items (options): */
    .select-items {
      position: absolute;
      background-color: #266788;
      top: 100%%;
      left: 0;
      right: 0;
      z-index: 99;
      
    }

    /* Hide the items when the select box is closed: */
    .select-hide {
      display: none;
    }

    .select-items div:hover, .same-as-selected {
      background-color: rgba(0, 0, 0, 0.1);
    } 

    .formquestion{
      color: #000000;

    }
    .formtypefield{ 
      position: relative;
      font-family: Arial;
      margin: 20px auto;
      background-color: #266788;
      border-radius: 10px;
      margin: 20px auto;
      width: 20%%;
      box-shadow: 10px 10px 20px rgba(0, 0, 0, 0.25);
      border-style: solid;
      border-width: 4px;
      height: 46px;
      border-color: #000000;
      color: white;
      font-size: 24px;
    }
    input::placeholder {
    color: white;
    opacity: 0.8;
    font-size: 24px;
    align-self: center;
  }
  .submitbutton{
    background-color: #266788;
    border-radius: 10px;
    margin: 20px auto;
    width: 20%%;
    box-shadow: 10px 10px 20px rgba(0, 0, 0, 0.25);
    border-style: solid;
    border-width: 4px;
    color: white;
    font-size: 24px;
    height: 46px;
    border-color: #000000;
    cursor: pointer;
  }

  </style>

</head>
<body>
  
  <h1 class="home ">Home</h1>
    <div class="addnewitemdiv" id="myBtn" >
        <p class="divboxtext">Add new item to display </p>
    </div>

  <h1 class="home ">Display <br> Sequence</h1>
  
  <div id="simpleList" class="list-group">

    %DISPLAYSEQUENCE%
  </div>


  <!-- The Modal -->
  <div id="myModal" class="modal">

    <div class="modal-content">
      <span class="close">&times;</span>
      <p>Add new item to display</p>


      <div class="custom-select" >
        <select id="displayitem">
          <option value="0">Select What to display</option>
          <option value="1">Stock Price</option>
          <option value="2">Stock News</option>
          <option value="3">Weather</option>
        </select>
      </div>

      <div id="stockinputfielddiv" style="display: none; margin-top: 10px;">
      <label for="stocksymbol">Stock Symbol</label>
      <br>
      <input type="text" class="formtypefield" id="stocksymbol" name="stocksymbol" placeholder="AAPL">
      </div>

      <div id="weatherinputfielddiv" style="display: none; margin-top: 10px;">
      <label for="longitude">Longitude</label>
      <br>
      <input type="text" class="formtypefield" id="longitude" name="longitude" placeholder="82.8628">

      <br>
      <label for="lattitude">Lattitude</label>
      <br>
      <input type="text" class="formtypefield" id="lattitude" name="lattitude" placeholder="135.0000">
      </div>
    <button type="button" class="submitbutton" id="submitnewdisplayitem" style="display: none;" onclick="createXMLHttpRequest()">Save</button> 
  </div>
  </div>

  
  <div id="editdisplayitem" class="modal">
    <!-- Modal content -->
    <div class="modal-content">
      <span class="close">&times;</span>
      <p>Modify Display Item</p>


      <div class="custom-selectedit" >
        <select id="displayitem">
          <option value="0">Select What to display</option>
          <option value="1">Stock Price</option>
          <option value="2">Stock News</option>
          <option value="3">Weather</option>
        </select>
      </div>

      <div id="stockinputfielddivedit" style="display: none; margin-top: 10px;">
      <label for="stocksymbol">Stock Symbol</label>
      <br>
      <input type="text" class="formtypefield" id="stocksymboledit" name="stocksymbol" placeholder="AAPL">
      </div>

      <div id="weatherinputfielddivedit" style="display: none; margin-top: 10px;">
      <label for="longitude">Longitude</label>
      <br>
      <input type="text" class="formtypefield" id="longitudeedit" name="longitude" placeholder="82.8628">

      <br>
      <label for="lattitude">Lattitude</label>
      <br>
      <input type="text" class="formtypefield" id="lattitudeedit" name="lattitude" placeholder="135.0000">
      </div>
    <button type="button" class="submitbutton" id="submitnewdisplayitemedit" style="display: none;" onclick="updateXMLHttpRequest()">Save</button> 
  </div>

  </div>


  <script>
    // Simple list
    Sortable.create(simpleList, { /* options */ });

    // Start of modal
    // Get the modal
    var modaledit = document.getElementById("editdisplayitem");



    // Get the <span> element that closes the modal
    var spanedit = document.getElementsByClassName("close")[1];

    // When the user clicks on the button, open the modal
    function editmodal(element) {
      modaledit.style.display = "block";
      modal.edititem = element;
      console.log(element.dataset.symbol);
      modaledit.dataset.uid = element.dataset.uid;
      

    }

    // When the user clicks on <span> (x), close the modal
    spanedit.onclick = function() {
      modaledit.style.display = "none";
      location.reload(true);

      
    }

    window.onclick = function(event) {
  if (event.target.classList.contains("modal")) {
    event.target.style.display = "none";
    location.reload(true);
  }
}

    // End of modal

  </script>
  
  


  <script>
    // Simple list
    Sortable.create(simpleList, { /* options */ });

    // Start of modal
    // Get the modal
    var modal = document.getElementById("myModal");

    // Get the button that opens the modal
    var btn = document.getElementById("myBtn");

    // Get the <span> element that closes the modal
    var span = document.getElementsByClassName("close")[0];

    // When the user clicks on the button, open the modal
    btn.onclick = function() {
      modal.style.display = "block";
    }

    // When the user clicks on <span> (x), close the modal
    span.onclick = function() {
      modal.style.display = "none";
      location.reload(true);

    }



  </script>

<script>
  var x, i, j, l, ll, selElmnt, a, b, c;
  /*look for any elements with the class "custom-select":*/
  x = document.getElementsByClassName("custom-select");
  l = x.length;
  for (i = 0; i < l; i++) {
    selElmnt = x[i].getElementsByTagName("select")[0];
    ll = selElmnt.length;
    /*for each element, create a new DIV that will act as the selected item:*/
    a = document.createElement("DIV");
    a.setAttribute("class", "select-selected");
    a.innerHTML = selElmnt.options[selElmnt.selectedIndex].innerHTML;
    x[i].appendChild(a);
    /*for each element, create a new DIV that will contain the option list:*/
    b = document.createElement("DIV");
    b.setAttribute("class", "select-items select-hide");
    for (j = 1; j < ll; j++) {
      /*for each option in the original select element,
      create a new DIV that will act as an option item:*/
      c = document.createElement("DIV");
      c.innerHTML = selElmnt.options[j].innerHTML;
      c.addEventListener("click", function(e) {
          /*when an item is clicked, update the original select box,
          and the selected item:*/
          var y, i, k, s, h, sl, yl;
          s = this.parentNode.parentNode.getElementsByTagName("select")[0];
          sl = s.length;
          h = this.parentNode.previousSibling;
          for (i = 0; i < sl; i++) {
            if (s.options[i].innerHTML == this.innerHTML) {
              s.selectedIndex = i;
              h.innerHTML = this.innerHTML;
              y = this.parentNode.getElementsByClassName("same-as-selected");
              yl = y.length;
              for (k = 0; k < yl; k++) {
                y[k].removeAttribute("class");
              }
              this.setAttribute("class", "same-as-selected");
              break;
            }
          }
          h.click();
      });
      b.appendChild(c);
    }
    x[i].appendChild(b);
    a.addEventListener("click", function(e) {
        /*when the select box is clicked, close any other select boxes,
        and open/close the current select box:*/
        e.stopPropagation();
        closeAllSelect(this);
        this.nextSibling.classList.toggle("select-hide");
        this.classList.toggle("select-arrow-active");
      });
  }
  function closeAllSelect(elmnt) {
    /*a function that will close all select boxes in the document,
    except the current select box:*/
    var x, y, i, xl, yl, arrNo = [];
    x = document.getElementsByClassName("select-items");
    y = document.getElementsByClassName("select-selected");
    xl = x.length;
    yl = y.length;
    for (i = 0; i < yl; i++) {
      if (elmnt == y[i]) {
        arrNo.push(i)
      } else {
        y[i].classList.remove("select-arrow-active");
      }
    }
    for (i = 0; i < xl; i++) {
      if (arrNo.indexOf(i)) {
        x[i].classList.add("select-hide");
      }
    }
  }
  /*if the user clicks anywhere outside the select box,
  then close all select boxes:*/
  document.addEventListener("click", closeAllSelect);
  </script>

<script>
  var x, i, j, l, ll, selElmnt, a, b, c;
  /*look for any elements with the class "custom-select":*/
  x = document.getElementsByClassName("custom-selectedit");
  l = x.length;
  for (i = 0; i < l; i++) {
    selElmnt = x[i].getElementsByTagName("select")[0];
    ll = selElmnt.length;
    /*for each element, create a new DIV that will act as the selected item:*/
    a = document.createElement("DIV");
    a.setAttribute("class", "select-selected");
    a.innerHTML = selElmnt.options[selElmnt.selectedIndex].innerHTML;
    x[i].appendChild(a);
    /*for each element, create a new DIV that will contain the option list:*/
    b = document.createElement("DIV");
    b.setAttribute("class", "select-items select-hide");
    for (j = 1; j < ll; j++) {
      /*for each option in the original select element,
      create a new DIV that will act as an option item:*/
      c = document.createElement("DIV");
      c.innerHTML = selElmnt.options[j].innerHTML;
      c.addEventListener("click", function(e) {
          /*when an item is clicked, update the original select box,
          and the selected item:*/
          var y, i, k, s, h, sl, yl;
          s = this.parentNode.parentNode.getElementsByTagName("select")[0];
          sl = s.length;
          h = this.parentNode.previousSibling;
          for (i = 0; i < sl; i++) {
            if (s.options[i].innerHTML == this.innerHTML) {
              s.selectedIndex = i;
              h.innerHTML = this.innerHTML;
              y = this.parentNode.getElementsByClassName("same-as-selected");
              yl = y.length;
              for (k = 0; k < yl; k++) {
                y[k].removeAttribute("class");
              }
              this.setAttribute("class", "same-as-selected");
              break;
            }
          }
          h.click();
      });
      b.appendChild(c);
    }
    x[i].appendChild(b);
    a.addEventListener("click", function(e) {
        /*when the select box is clicked, close any other select boxes,
        and open/close the current select box:*/
        e.stopPropagation();
        closeAllSelect(this);
        this.nextSibling.classList.toggle("select-hide");
        this.classList.toggle("select-arrow-active");
      });
  }
  function closeAllSelect(elmnt) {
    /*a function that will close all select boxes in the document,
    except the current select box:*/
    var x, y, i, xl, yl, arrNo = [];
    x = document.getElementsByClassName("select-items");
    y = document.getElementsByClassName("select-selected");
    xl = x.length;
    yl = y.length;
    for (i = 0; i < yl; i++) {
      if (elmnt == y[i]) {
        arrNo.push(i)
      } else {
        y[i].classList.remove("select-arrow-active");
      }
    }
    for (i = 0; i < xl; i++) {
      if (arrNo.indexOf(i)) {
        x[i].classList.add("select-hide");
      }
    }
  }
  /*if the user clicks anywhere outside the select box,
  then close all select boxes:*/
  document.addEventListener("click", closeAllSelect);
</script>


<script>
// Wait for DOM to be ready
document.addEventListener("DOMContentLoaded", function () {
  const options = document.querySelectorAll(".select-items div");

  options.forEach(function (optionDiv) {
    optionDiv.addEventListener("click", function () {
      const selectedText = this.innerText.trim();

      const stock = document.getElementById("stockinputfielddiv");
      const weather = document.getElementById("weatherinputfielddiv");
      const submitbutton = document.getElementById("submitnewdisplayitem");


      // Hide all first
      stock.style.display = "none";
      weather.style.display = "none";
      submitbutton.style.display = "none";


      

      // Show based on selected text
      if (selectedText === "Stock Price"  ) {
        stock.style.display = "block";
        document.getElementById('submitnewdisplayitem').onclick=() => createXMLHttpRequest('StockPrice') 
      } else if (selectedText === "Stock News") {
        stock.style.display = "block";
        document.getElementById('submitnewdisplayitem').onclick=() => createXMLHttpRequest('StockNews') 
      } else if (selectedText === "Weather") {
        weather.style.display = "block";
        document.getElementById('submitnewdisplayitem').onclick=() => createXMLHttpRequest('Weather');
      }
      submitbutton.style.display = "block";
      // submitbutton.attribute

    });
  });
});

document.addEventListener("DOMContentLoaded", function () {
  const options = document.querySelectorAll(".select-items div");

  options.forEach(function (optionDiv) {
    optionDiv.addEventListener("click", function () {
      const selectedText = this.innerText.trim();

      const stockedit = document.getElementById("stockinputfielddivedit");
      const weatheredit = document.getElementById("weatherinputfielddivedit");
      const submitbuttonedit = document.getElementById("submitnewdisplayitemedit");


      // Hide all first
      stockedit.style.display = "none";
      weatheredit.style.display = "none";
      submitbuttonedit.style.display = "none";


      

      // Show based on selected text
      if (selectedText === "Stock Price"  ) {
        stockedit.style.display = "block";
        document.getElementById('submitnewdisplayitemedit').onclick=() => updateXMLHttpRequest('StockPrice',modaledit.dataset.uid) 
      } else if (selectedText === "Stock News") {
        stockedit.style.display = "block";
        document.getElementById('submitnewdisplayitemedit').onclick=() => updateXMLHttpRequest('StockNews',modaledit.dataset.uid) 
      } else if (selectedText === "Weather") {
        weatheredit.style.display = "block";
        document.getElementById('submitnewdisplayitemedit').onclick=() => updateXMLHttpRequest('Weather',modaledit.dataset.uid);
      }
      submitbuttonedit.style.display = "block";
      // submitbutton.attribute

    });
  });
});

</script>


<script>
  function createXMLHttpRequest(category) {
    console.log(category);
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
      if (xhttp.readyState === 4 && xhttp.status === 200) {
        // ✅ Request successful, refresh the page
        location.reload();
      }
    };

    if (category == 'StockPrice' || category == 'StockNews') {
      var stockname = document.getElementById("stocksymbol").value;
      console.log("/create?category=" + category + "&stockname=" + stockname);
      xhttp.open("GET", "/create?category=" + category + "&stockname=" + stockname, true);
    } else if (category == 'weather') {
      var longitude = document.getElementById("longitude").value;
      var lattitude = document.getElementById("lattitude").value;
      xhttp.open("GET", "/create?category=" + category + "&longitude=" + longitude + "&lattitude=" + lattitude, true);
    }

    xhttp.send();
  }
</script>

<script>
  function updateXMLHttpRequest(category,uid) {
    console.log(category);
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
      if (xhttp.readyState === 4 && xhttp.status === 200) {
        // ✅ Request successful, refresh the page
        location.reload();
      }
    };

    if (category == 'StockPrice' || category == 'StockNews') {
      var stockname = document.getElementById("stocksymboledit").value;
      console.log("/update?category=" + category + "&stockname=" + stockname);
      xhttp.open("GET", "/update?category=" + category + "&stockname=" + stockname +"&uid="+uid, true);
    } else if (category == 'weather') {
      var longitude = document.getElementById("longitudeedit").value;
      var lattitude = document.getElementById("lattitudeedit").value;
      xhttp.open("GET", "/update?category=" + category + "&longitude=" + longitude + "&lattitude=" + lattitude +"&uid="+uid , true);
    }

    xhttp.send();
  }
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  display.init(115200,true,50,false);
  Setupwifi();
  ayncserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html,processor);
  });


  ayncserver.on("/create", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String category;
    String stockname;
    String longitude;
    String lattitude;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(serverparamcategory) && request->hasParam(serverparamstockname)) {
      
      category = request->getParam(serverparamcategory)->value();
      if(category == "StockPrice" || category == "StockNews"){
        stockname = request->getParam(serverparamstockname)->value();
        saveArrayWithUUID({ category, stockname });
        Serial.print("Saved data: ");
        Serial.print(category);
    
      };
    }
    else if(request->hasParam(serverparamcategory) && request->hasParam(serverparamlattitude) && request->hasParam(serverparamlongitude)){
      category = request->getParam(serverparamcategory)->value();
      if(category == "Weather" ){
        lattitude = request->getParam(serverparamlattitude)->value();
        longitude = request->getParam(serverparamlongitude)->value();
        saveArrayWithUUID({ category, lattitude,longitude  });
        Serial.print("Saved data: ");
        Serial.print(category);
    
      };
      };
    


    request->send(200, "text/plain", "OK");
  });

  ayncserver.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String category;
    String stockname;
    String longitude;
    String lattitude;
    String uid;
    Serial.println("Update request received");
    Serial.println(request->hasParam(serverparamcategory));
    Serial.println(request->hasParam(serverparamstockname));
    Serial.println(request->hasParam(uid));
    

    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(serverparamcategory) && request->hasParam(serverparamstockname) && request->hasParam(serverparamuid)) {
      Serial.println("Update request received");

      category = request->getParam(serverparamcategory)->value();
      uid = request->getParam(serverparamuid)->value();

      if(category == "StockPrice" || category == "StockNews"){
        stockname = request->getParam(serverparamstockname)->value();
        updateArrayWithUUID({ category, stockname },uid);
        Serial.print("Saved data: ");
        Serial.print(category);
    
      };
    };

    


    request->send(200, "text/plain", "OK");
  });


  ayncserver.begin();
  pixels.begin();
  GetWeatherData(103.8501,1.2897);
  preferences.begin("my-app", false);
  preferences.clear();
  if (preferences.isKey("allUUIDs")==false) {
    preferences.putString("allUUIDs", "");  } 
}

void loop() {
  // GetStockYahooFinanceHttps("AXON");
  // delay(5000);
  // helloWorld("AXON",stockprice.currentPrice,stockprice.percentageChange);
  // setpixelcolourstock();
  // GetStockNews("AAPL");
  // delay(5000);
  // displayStockNews("AAPL",stockprice.newstitle,stockprice.newssource);
  // GetStockYahooFinanceHttps("AAPL");
  // delay(5000);
  // helloWorld("AAPL",stockprice.currentPrice,stockprice.percentageChange);
  
  displayiteminsequence();
  delay(5000);
}

// put function definitions here:

int Setupwifi() {
  WiFiManager wm;
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("AutoConnectAP"); // anonymous ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }
  return 0;
}


int GetStockDataAlphaVantageHttps(String symbol) {
  const char*  server = "www.alphavantage.co";  // Server URL
  const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD\n" \
  "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG\n" \
  "A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw\n" \
  "WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz\n" \
  "IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi\n" \
  "AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi\n" \
  "QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR\n" \
  "HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW\n" \
  "BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D\n" \
  "9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8\n" \
  "p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD\n" \
  "-----END CERTIFICATE-----\n";
  WiFiClientSecure client;
  client.setCACert(test_root_ca);
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");

    else {
      Serial.println("Connected to server!");
      Serial.println("https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + symbol + "&interval=5min&apikey=A0OFEISI3OBSH41P HTTP/1.0");

      // Make a HTTP request:
      client.println("GET https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + symbol + "&interval=5min&apikey=A0OFEISI3OBSH41P HTTP/1.0");
      client.println("Host: www.alphavantage.co");
      client.println("Connection: close");
      client.println();

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          Serial.println("headers received");
          break;
        }
      }
      // if there are incoming bytes available
      // from the server, read them and print them:
      String response = "";  // Declare response as a String to store the entire response

      while (client.available()) {
          response += (char)client.read();  // Read and append each byte to the response string
      }
      client.stop();
      Serial.println(response);
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);
    
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return 0;
      }

      Serial.println(doc["Global Quote"]["01. symbol"].as<String>());
      Serial.println(doc["Global Quote"]["02. open"].as<String>());
    }
  return 0;
}

int GetStockYahooFinanceHttps(String symbol) {
  String YahooFinance = "https://finance.yahoo.com/quote/" + symbol;

  // HTTP headers for the request
  String userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36";
  String cookie = "B=a2qoc01dom2ob&b=3&s=hv; GUC=AQEBAQFbyD5cnEIe4gR7&s=AQAAAKXb1OzH&g=W8bwqg; PRF=t%3DNFLX%252BSPY%252BRTY%253DF%252BRUTH%252B%255EIXIC%26fin-trd-cue%3D1";
  String referer = "https://finance.yahoo.com/quote/"+ symbol;
  String pragma = "no-cache";
  String upgradeInsecureRequests = "1";
  float percentageChange;
  float currentPrice;
  float PreviousClose;


  const char*  server = "finance.yahoo.com";  // Server URL
  const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
  "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
  "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
  "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
  "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
  "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
  "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
  "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
  "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
  "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
  "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
  "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
  "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
  "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
  "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
  "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
  "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
  "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
  "+OkuE6N36B9K\n" \
  "-----END CERTIFICATE-----\n";
  WiFiClientSecure client;
  client.setCACert(test_root_ca);
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");

    else {
      Serial.println("Connected to server!");
      Serial.println("https://query1.finance.yahoo.com/v8/finance/chart/"+ symbol+"?interval=1d&range=1d HTTP/1.0");

      client.println("GET https://query1.finance.yahoo.com/v8/finance/chart/"+ symbol+"?interval=1d&range=1d HTTP/1.1");  // HTTP GET request
      client.println("Host: finance.yahoo.com");  // Host header
      client.println("User-Agent: " + userAgent);  // User-Agent header
      client.println("Cookie: " + cookie);  // Cookie header
      client.println("Pragma: " + pragma);  // Pragma header
      client.println("Referer: " + referer);  // Referer header
      client.println("Upgrade-Insecure-Requests: " + upgradeInsecureRequests);  // Upgrade-Insecure-Requests header
      client.println("Connection: close");  // Close the connection after the request
      client.println();  // End the headers section with a blank line
  

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          Serial.println("headers received");
          break;
        }
      }
      // if there are incoming bytes available
      // from the server, read them and print them:
      String response = "";  // Declare response as a String to store the entire response

      while (client.available()) {
          response += (char)client.read();  // Read and append each byte to the response string
      }
      client.stop();
      Serial.println(response);
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);
    
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return 0;
      }

      currentPrice=doc["chart"]["result"][0]["meta"]["regularMarketPrice"].as<float>();
      PreviousClose=doc["chart"]["result"][0]["meta"]["chartPreviousClose"].as<float>();
      percentageChange= currentPrice/PreviousClose*100-100;
      Serial.println(percentageChange);
      Serial.println(currentPrice);

      stockprice.openPrice=doc["chart"]["result"][0]["indicators"]["quote"][0]['open'].as<float>();
      stockprice.highPrice=doc["chart"]["result"][0]["indicators"]["quote"][0]['high'].as<float>();
      stockprice.lowPrice=doc["chart"]["result"][0]["indicators"]["quote"][0]['low'].as<float>();
      stockprice.closePrice=doc["chart"]["result"][0]["indicators"]["quote"][0]['close'].as<float>();
    }
    stockprice.currentPrice=currentPrice;
    stockprice.percentageChange=percentageChange;
  return 0;
}


int helloWorld(String symbol,float currentPrice,float percentageChange) { 
  display.setRotation(1);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setFullWindow();
  display.firstPage();
  display.setCursor(0, 30);
  do
    {
    display.setTextColor(GxEPD_BLACK);
    display.print(symbol);
    // display.fillRect(0, 0, display.width(), 45, GxEPD_BLACK);
    display.setCursor(0, 25);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(0, 70);
    display.print(currentPrice);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0, 105);
    if (stockprice.percentageChange> 0){
      display.print("+"+String(percentageChange)+"%");
    }
    else{
      display.print(""+String(percentageChange)+"%");
    };
    
    // display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    // display.getTextBounds(HelloWeACtStudio, 0, 0, &tbx, &tby, &tbw, &tbh);
    // x = ((display.width() - tbw) / 2) - tbx;
    // display.setCursor(x, y+tbh);
  }

  while (display.nextPage());
  return 0;
}

int setpixelcolourstock(){
  pixels.clear();
  for(int i=0; i<2; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if (stockprice.percentageChange> 0){
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    };

    pixels.show();   // Send the updated pixel colors to the hardware.

    // delay(1000); // Pause before next pass through loop
  };
  return 0;
};

int GetLatestPackageStatus(String trackingid) {
  const char* server = "api.ordertracker.com";  // Server URL
  const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD\n"\
  "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG\n"\
  "A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw\n"\
  "WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz\n"\
  "IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi\n"\
  "AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi\n"\
  "QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR\n"\
  "HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW\n"\
  "BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D\n"\
  "9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8\n"\
  "p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD\n"\
  "-----END CERTIFICATE-----\n";
  WiFiClientSecure client;
  client.setCACert(test_root_ca);
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");

    else {
      Serial.println("Connected to server!");
      Serial.println("https://api.ordertracker.com/public/track/"+ trackingid+" HTTP/1.0");

      client.println("GET https://api.ordertracker.com/public/track/"+ trackingid+" HTTP/1.1");  // HTTP GET request
      client.println("Connection: close");  // Close the connection after the request
      client.println();  // End the headers section with a blank line
  

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          Serial.println("headers received");
          break;
        }
      }
      // if there are incoming bytes available
      // from the server, read them and print them:
      String response = "";  // Declare response as a String to store the entire response

      while (client.available()) {
          response += (char)client.read();  // Read and append each byte to the response string
      }
      client.stop();
      Serial.println(response);
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);
    
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return 0;
      }


    }
    return 0;
}


int GetWeatherData(float longitide,float latitude) {
  const char*  server = "api.open-meteo.com";  // Server URL
  const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n"\
  "-----END CERTIFICATE-----\n";
  WiFiClientSecure client;
  client.setCACert(test_root_ca);
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");

    else {
      Serial.println("Connected to server!");
      Serial.println("https://api.open-meteo.com/v1/forecast?latitude="+String(latitude)+"&longitude="+String(longitide)+"&current=temperature_2m,relative_humidity_2m,apparent_temperature,rain,precipitation,showers HTTP/1.0");

      client.println("GET https://api.open-meteo.com/v1/forecast?latitude="+String(latitude)+"&longitude="+String(longitide)+"&current=temperature_2m,relative_humidity_2m,apparent_temperature,rain,precipitation,showers HTTP/1.0");  // HTTP GET request
      client.println("Connection: close");  // Close the connection after the request
      client.println();  // End the headers section with a blank line
  

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          Serial.println("headers received");
          break;
        }
      }
      // if there are incoming bytes available
      // from the server, read them and print them:
      String response = "";  // Declare response as a String to store the entire response

      while (client.available()) {
          response += (char)client.read();  // Read and append each byte to the response string
      }
      client.stop();
      Serial.println(response);
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);
    
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return 0;
      }


    }
    return 0;
}


int GetStockNews(String symbol) {
  const char*  server = "api.tickertick.com";  // Server URL
  const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD\n"\
  "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG\n"\
  "A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw\n"\
  "WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz\n"\
  "IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi\n"\
  "AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi\n"\
  "QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR\n"\
  "HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW\n"\
  "BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D\n"\
  "9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8\n"\
  "p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD\n"\
  "-----END CERTIFICATE-----\n";
  WiFiClientSecure client;
  client.setCACert(test_root_ca);
  String path = "/feed?q=%28and%20%28or%20T%3Acurated%20T%3Aanalysis%20T%3Atrade%20T%3Amarket%20T%3Aindustry%20T%3Asec_fin%20T%3Asec%20T%3Aearning%29%20tt%3A" + symbol + "%29&n=1";
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");

    else {
      Serial.println("Connected to server!");
      Serial.println("GET " + path + " HTTP/1.1");
      client.println("GET " + path + " HTTP/1.1");
      client.println("Host: api.tickertick.com");
      client.println("User-Agent: ESP32");
      client.println("Accept: application/json");
      client.println("Connection: close");
      client.println();  // End the headers section with a blank line

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        if (line == "\r") {
          Serial.println("headers received");
          break;
        }
      }
      // if there are incoming bytes available
      // from the server, read them and print them:
      String response = "";  // Declare response as a String to store the entire response

      while (client.available()) {

          response += (char)client.read();  // Read and append each byte to the response string
      }
      client.stop();
      Serial.println(response);
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);
    
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return 0;
      }
      stockprice.newstitle=doc["stories"][0]["title"].as<String>();
      stockprice.newssource=doc["stories"][0]["site"].as<String>();

    }
    return 0;
}


int displayStockNews(String symbol,String title,String source) { 
  display.setRotation(1);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setFullWindow();
  display.firstPage();
  display.setCursor(0, 15);
  do
    {
    display.setTextColor(GxEPD_BLACK);
    display.print(symbol);
    // display.fillRect(0, 0, display.width(), 45, GxEPD_BLACK);
    // display.setCursor(0, 15);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0, 40);
    display.print(source);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(0, 65);
    display.print(title);

    
    // display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    // display.getTextBounds(HelloWeACtStudio, 0, 0, &tbx, &tby, &tbw, &tbh);
    // x = ((display.width() - tbw) / 2) - tbx;
    // display.setCursor(x, y+tbh);
  }

  while (display.nextPage());
  return 0;
}


void saveArrayWithUUID(std::vector<String> data) {
  String uuid = generateUID();
  String key =  uuid;

  String combined = "";
  for (int i = 0; i < data.size(); i++) {
    combined += data[i];
    if (i != data.size() - 1)
      combined += "|";
  }

  preferences.putString(key.c_str(), combined);
  addUUID(uuid);
  Serial.println("Saved to key: " + key);
}

void updateArrayWithUUID(std::vector<String> data, const String& uuid) {
  String combined = "";
  for (int i = 0; i < data.size(); i++) {
    combined += data[i];
    if (i != data.size() - 1)
      combined += "|";
  }

  preferences.putString(uuid.c_str(), combined);
  Serial.println("Updated key: " + uuid);
}



std::vector<String> loadArrayWithUUID(const String& uuid) {
  String key = uuid;
  String value = preferences.getString(key.c_str(), "");
  std::vector<String> result;
  
  int start = 0;
  int delimIndex;
  while ((delimIndex = value.indexOf('|', start)) != -1) {
    result.push_back(value.substring(start, delimIndex));
    start = delimIndex + 1;
  }
  if (start < value.length())
    result.push_back(value.substring(start));
    
  return result;
}


String processor(const String& var){
  Serial.println(var);
  if(var == "DISPLAYSEQUENCE"){
    String displayitems = "";
    String uuidList = preferences.getString("allUUIDs", "");
    Serial.println(uuidList);
    if (uuidList != "") {
      // Split the string by the delimiter '|'
      int start = 0;
      int delimiterIndex;
      
      // Iterate through the list of UUIDs
      while ((delimiterIndex = uuidList.indexOf('|', start)) != -1) {
        String uuid = uuidList.substring(start, delimiterIndex);
        std::vector<String> entry = loadArrayWithUUID(uuid);
        Serial.println(uuid);

        if (entry[0] == "StockPrice") {
          displayitems+= "<div class=\"divbox list-group-item\"><p onclick=\"editmodal(this)\" class=\"divboxtext\" data-uid=\""+uuid+"\">Stock Price "+entry[1]+"</p></div>";
        } else if (entry[0] == "StockNews") { 
          displayitems+= "<div class=\"divbox list-group-item\"><p onclick=\"editmodal(this)\" class=\"divboxtext\" data-uid=\""+uuid+"\">Stock News "+entry[1]+"</p></div>";
        }
        start = delimiterIndex + 1;
      }
      
      // Don't forget to handle the last UUID after the last delimiter
      String lastUUID = uuidList.substring(start);
      std::vector<String> entry = loadArrayWithUUID(lastUUID);
      Serial.println(lastUUID);
      if (entry[0] == "StockPrice") {
        displayitems+= "<div class=\"divbox list-group-item\"><p onclick=\"editmodal(this)\" class=\"divboxtext\" data-uid=\""+lastUUID+"\">Stock Price "+entry[1]+"</p></div>";
      } else if (entry[0] == "StockNews") { 
        displayitems+= "<div class=\"divbox list-group-item\"><p onclick=\"editmodal(this)\" class=\"divboxtext\" data-uid=\""+lastUUID+"\">Stock News "+entry[1]+"</p></div>";
      }
    } else {
      Serial.println("No UUIDs found.");
    };

      return displayitems;
    }
  return String();
}

String generateUID() {
  char uid[11];  // 10 characters + null terminator
  sprintf(uid, "%010X", esp_random() & 0x3FFFFFFF);  // 30 bits max = 8 digits, pad to 10
  return String(uid);
}

void addUUID(String newUUID) {
  // Open preferences with namespace "UUID_storage"

  // Retrieve the current list of UUIDs
  String uuidString = "";
  if (preferences.isKey("allUUIDs")) {
    uuidString = preferences.getString("allUUIDs");
  }
  
  // If there's an existing list, append the new UUID with a delimiter (e.g., pipe '|')
  if (uuidString != "") {
    uuidString += "|" + newUUID;  // Add new UUID with pipe delimiter
  } else {
    uuidString = newUUID;  // If no UUIDs exist, start the list with the new UUID
  }

  // Save the updated list back to preferences
  preferences.putString("allUUIDs", uuidString);

  Serial.println("New UUID added: " + newUUID);
}


void displayiteminsequence() {
  String uuidList = preferences.getString("allUUIDs", "");
  Serial.println(uuidList);
  if (uuidList != "") {
    // Split the string by the delimiter '|'
    int start = 0;
    int delimiterIndex;
    
    // Iterate through the list of UUIDs
    while ((delimiterIndex = uuidList.indexOf('|', start)) != -1) {
      String uuid = uuidList.substring(start, delimiterIndex);
      std::vector<String> entry = loadArrayWithUUID(uuid);
      Serial.println(uuid);

      if (entry[0] == "StockPrice") {
        GetStockYahooFinanceHttps(entry[1]);
        helloWorld(entry[1],stockprice.currentPrice,stockprice.percentageChange);
      } else if (entry[0] == "StockNews") { 
        GetStockNews(entry[1]);
        displayStockNews(entry[1],stockprice.newstitle,stockprice.newssource);
      }
      start = delimiterIndex + 1;
    }
    
    // Don't forget to handle the last UUID after the last delimiter
    String lastUUID = uuidList.substring(start);
    std::vector<String> entry = loadArrayWithUUID(lastUUID);
    Serial.println(lastUUID);
    if (entry[0] == "StockPrice") {
      GetStockYahooFinanceHttps(entry[1]);
      helloWorld(entry[1],stockprice.currentPrice,stockprice.percentageChange);
    } else if (entry[0] == "StockNews") { 
      GetStockNews(entry[1]);
      displayStockNews(entry[1],stockprice.newstitle,stockprice.newssource);
    }
    delay(5000);
  } else {
    Serial.println("No UUIDs found.");
  };
}

