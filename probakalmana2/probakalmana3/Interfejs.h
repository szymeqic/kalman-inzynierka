const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="pl-PL" class="js-focus-visible">
<head>
   <meta charset="UTF-8">
  </head>

<title>Filtr Kalmana praca inżynierska</title>

<style>
table {
    position: relative;
    width:100%;
    border-spacing: 0px;
  }
  tr {
    border: 1px solid white;
    font-family: "Verdana", "Arial", sans-serif;
    font-size: 20px;
  }
  th {
    height: 20px;
    padding: 3px 15px;
    background-color: #343a40;
    color: #FFFFFF !important;
    }
  td {
    height: 20px;
     padding: 3px 15px;
  }
  .tabledata {
    font-size: 24px;
    position: relative;
    padding-left: 5px;
    padding-top: 5px;
    height:   25px;
    border-radius: 5px;
    color: #FFFFFF;
    line-height: 20px;
    transition: all 200ms ease-in-out;
    background-color: #00AA00;
  }
  .fanrpmslider {
    width: 90%;
    height: 55px;
    outline: none;
    height: 25px;
  }
  .bodytext {
    font-family: "Verdana", "Times New Roman", sans-serif;
    font-size: 24px;
    text-align: left;
    font-weight: light;
    border-radius: 5px;
    display:inline;
  }
  .navbar {
    width: 100%;
    height: 50px;
    margin: 0;
    padding: 10px 0px;
    background-color: #FFF;
    color: #000000;
    border-bottom: 5px solid #293578;
  }

  .column {
    float :left;
    width : 50%;
    
  }

  .row:after {
      content :"";
      display: table;
      clear: both;

  }
  .fixed-top {
    position: fixed;
    top: 0;
    right: 0;
    left: 0;
    z-index: 1030;
  }
  .navtitle {
    float: left;
    height: 50px;
    font-family: "Verdana", "Arial", sans-serif;
    font-size: 50px;
    font-weight: bold;
    line-height: 50px;
    padding-left: 20px;
  }
 .navheading {
   position: fixed;
   left: 60%;
   height: 50px;
   font-family: "Verdana", "Arial", sans-serif;
   font-size: 20px;
   font-weight: bold;
   line-height: 20px;
   padding-right: 20px;
 }
 .navdata {
    justify-content: flex-end;
    position: fixed;
    left: 70%;
    height: 50px;
    font-family: "Verdana", "Arial", sans-serif;
    font-size: 20px;
    font-weight: bold;
    line-height: 20px;
    padding-right: 20px;
 }
  .category {
    font-family: "Verdana", "Arial", sans-serif;
    font-weight: bold;
    font-size: 32px;
    line-height: 50px;
    padding: 20px 10px 0px 10px;
    color: #000000;
  }
  .heading {
    font-family: "Verdana", "Arial", sans-serif;
    font-weight: normal;
    font-size: 28px;
    text-align: left;
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
    float: center;
  }
  .foot {
    font-family: "Verdana", "Arial", sans-serif;
    font-size: 20px;
    position: relative;
    height:   30px;
    text-align: center;   
    color: #AAAAAA;
    line-height: 20px;
  }
  .container {
    max-width: 1800px;
    margin: 0 auto;
  }
  #myProgress {
    width: 20%;
    height: 30px;
    position: relative;
    background-color: #ddd;
  }
  #myBar {
    background-color: #0000FF;
    width: 10px;
    height: 30px;
    position: absolute;
  }
  table tr:first-child th:first-child {
    border-top-left-radius: 5px;
  }
  table tr:first-child th:last-child {
    border-top-right-radius: 5px;
  }
  table tr:last-child td:first-child {
    border-bottom-left-radius: 5px;
  }
  table tr:last-child td:last-child {
    border-bottom-right-radius: 5px;
  }

</style>

  <body style="background-color: #efefef" onload="process()">

    <header>
      <div class="navbar fixed-top">
          <div class="container">
            <div class="navtitle">Panel sterowniczy drona na uwięzi</div>
            <div class="navdata" id = "date">mm/dd/yyyy</div>
            <div class="navheading">DATA</div><br>
            <div class="navdata" id = "time">00:00:00</div>
            <div class="navheading">GODZINA</div>
            
          </div>
      </div>
    </header>
  
    <main class="container" style="margin-top:70px">
      <div class ="row">
        <div class="column">
      <div class="category">Odczyty czujnika MPU6050</div>
    <br>
    <div class="bodytext">Obrót wokół osi X: <div class="bodytext" id="osx">123</div></div>
    <br>
    <div class="bodytext">Obrót wokół osi Y: <div class="bodytext" id="osy">123</div></div>
    <br>
    <br>
    <button type="button" class="btn" id="kalibracja" onclick="Kalibracja()">Kalibracja czujnika</button>
    <br>
    <br>
    <div id="myProgress">
      <div id="myBar"></div>
    </div>
    <br>
    <div class="category">Wartości zadane</div>
    <br>
    <div class="bodytext">Zadana wysokość: <span id="wysokosc"></span> cm</div>
    <br>
    <input type="range" class="fanrpmslider" min="5" max="40" value = "5" width = "0%" oninput="UpdateWysokosc(this.value)"/>
    <br>
    <div class="bodytext">Zadany kąt: <span id="kat"></span> stopni</div>
    <br>
    <input type="range" class="fanrpmslider" min="-65" max="65" value = "0" width = "0%" oninput="UpdateKat(this.value)"/>
    <br>
    <br>


    <div class="category">Ręczne sterowanie wypełnieniem PWM</div>
    <br>
    <div class="bodytext">Zadane wypełnienie silnika 1: <span id="PWM_1"></span> &mu;s</div>
    <br>
    <input type="range" class="fanrpmslider" min="1000" max="2000" value = "1000" width = "0%" id ="Pasek_PWM_1" oninput="Aktualizuj_PWM(this.value, 1)"/>
    <br>
    <div class="bodytext">Zadane wypełnienie silnika 2: <span id="PWM_2"></span> &mu;s</div>
    <br>
    <input type="range" class="fanrpmslider" min="1000" max="2000" value = "1000" width = "0%" id = "Pasek_PWM_2" oninput="Aktualizuj_PWM(this.value, 2)"/>

    <br>
    <div class="bodytext">Zadane wypełnienie obu silników: <span id="PWM_oba"></span> &mu;s</div>
    <br>
    <input type="range" class="fanrpmslider" min="1000" max="2000" value = "1000" width = "0%" id = "Pasek_PWM_oba" oninput="Aktualizuj_PWM(this.value,3)"/>
    <br>
    <br>
      </div>
      <div class = "column">

        <div class="category">Ręczne nastawy regulatora PID</div>
        <br>
          <div class = "column"> 
  
              <div class = "bodytext" style = "font-size: 30px;">Regulacja wysokości</div>
              <br>
              <br>
              <div class ="bodytext"> Współczynnik k<sub>p</sub>: <span id="Pasek_wys_kp_war"></span>  </div> 
              <input type="range" class="fanrpmslider" min="0.01" max="15" value = "0.01" width = "0%" step = "0.01" id = "Pasek_wys_kp" oninput="Pasek_PID(this.id, this.value)"/>
              <br>

              <br>
              <div class ="bodytext"> Współczynnik k<sub>i</sub>:  <span id="Pasek_wys_ki_war"></span>  </div> 
              <input type="range" class="fanrpmslider" min="0" max="5" value = "0.01" width = "0%" step = "0.01" id = "Pasek_wys_ki" oninput="Pasek_PID(this.id, this.value)"/>
              <br>

              <br>
              <div class ="bodytext"> Współczynnik k<sub>d</sub>: <span id="Pasek_wys_kd_war"></span>   </div> 
              <input type="range" class="fanrpmslider" min="0" max="5" value = "0.01" width = "0%" step = "0.01" id = "Pasek_wys_kd" oninput="Pasek_PID(this.id, this.value)"/>
              <br>

          </div>
        <div class = "column">
          <div class = "bodytext" style = "font-size: 30px;">Regulacja kąta</div>
          <br>
          <br>
          <div class ="bodytext"> Współczynnik k<sub>p</sub>: <span id="Pasek_kat_kp_war"></span>  </div> 
          <input type="range" class="fanrpmslider" min="0.01" max="15" value = "0.01" width = "0%" step = "0.01" id = "Pasek_kat_kp" oninput="Pasek_PID(this.id, this.value)"/>
          <br>

          <br>
          <div class ="bodytext"> Współczynnik k<sub>i</sub>:  <span id="Pasek_kat_ki_war"></span>  </div> 
          <input type="range" class="fanrpmslider" min="0" max="5" value = "0.01" width = "0%" step = "0.01" id = "Pasek_kat_ki" oninput="Pasek_PID(this.id, this.value)"/>
          <br>

          <br>
          <div class ="bodytext"> Współczynnik k<sub>d</sub>: <span id="Pasek_kat_kd_war"></span>   </div> 
          <input type="range" class="fanrpmslider" min="0" max="5" value = "0.01" width = "0%" step = "0.01" id = "Pasek_kat_kd" oninput="Pasek_PID(this.id, this.value)"/>
          <br>
      </div>
      </div>
    </div>
  </main>

  <footer div class="foot" id = "temp" >Szymon Kowalski & Bartosz Krajewski</div></footer>
  
  </body>

  <script>
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }


    /*
    function KalibracjaSilnika1() {
      var xhttp = new XMLHttpRequest(); 
      if (document.getElementById("kalibracja_silnika1").innerHTML == "Podaj najmniejszy PWM silnika 1"){
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja_silnika1").innerHTML = "Podaj największy PWM silnika 1";
        }
      }
      
      xhttp.open("PUT", "BUTTON_PWM_MIN1", false);
      xhttp.send(); 
      }

      else if (document.getElementById("kalibracja_silnika1").innerHTML == "Podaj największy PWM silnika 1"){
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja_silnika1").innerHTML = "Podaj zerowy PWM silnika 1";
        }
      }
      
      xhttp.open("PUT", "BUTTON_PWM_MAX1", false);
      xhttp.send(); 
      }

      else if (document.getElementById("kalibracja_silnika1").innerHTML == "Podaj zerowy PWM silnika 1"){
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja_silnika1").innerHTML = "Podaj najmniejszy PWM silnika 1";
        }
      }
      
      xhttp.open("PUT", "BUTTON_PWM_ZERO1", false);
      xhttp.send(); 
      }

    }

    function KalibracjaSilnika2() {
      var xhttp = new XMLHttpRequest(); 
      if (document.getElementById("kalibracja_silnika2").innerHTML == "Podaj najmniejszy PWM silnika 2"){
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja_silnika2").innerHTML = "Podaj największy PWM silnika 2";
        }
      }
      
      xhttp.open("PUT", "BUTTON_PWM_MIN2", false);
      xhttp.send(); 
      }

      else if (document.getElementById("kalibracja_silnika2").innerHTML == "Podaj największy PWM silnika 2"){
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja_silnika2").innerHTML = "Podaj zerowy PWM silnika 2";
        }
      }
      
      xhttp.open("PUT", "BUTTON_PWM_MAX2", false);
      xhttp.send(); 
      }

      else if (document.getElementById("kalibracja_silnika2").innerHTML == "Podaj zerowy PWM silnika 2"){
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja_silnika2").innerHTML = "Podaj najmniejszy PWM silnika 2";
        }
      }
      
      xhttp.open("PUT", "BUTTON_PWM_ZERO2", false);
      xhttp.send(); 
      }

    }
*/
    function Kalibracja() {
      var xhttp = new XMLHttpRequest(); 
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("kalibracja").innerHTML = this.responseText;
        }
      }
      */
      xhttp.open("PUT", "BUTTON_KALIBRACJA", false);
      xhttp.send(); 
      move();
    }

    function move() {
      const element = document.getElementById("myBar");
      let width = 0;
      const id = setInterval(frame, 37);
      function frame() {
        if(width == 100){
          clearInterval(id);
        }
        else {
          width++;
          element.style.width = width + '%';
        }
      }
    }
    
    function UpdateWysokosc(value) {
      var xhttp = new XMLHttpRequest();
      // this time i want immediate feedback to the fan speed
      // yea yea yea i realize i'm computing fan speed but the point
      // is how to give immediate feedback
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from  ESP
          document.getElementById("wysokosc").innerHTML=this.responseText;
        }
      }
      // this syntax is really weird the ? is a delimiter
      // first arg UPDATE_SLIDER is use in the .on method
      // server.on("/UPDATE_SLIDER", UpdateSlider);
      // then the second arg VALUE is use in the processing function
      // String t_state = server.arg("VALUE");
      // then + the controls value property
      xhttp.open("PUT", "UPDATE_WYSOKOSC?WYSOKOSC="+value, true);
      xhttp.send();
    }
    function UpdateKat(value) {
      var xhttp = new XMLHttpRequest();
      // this time i want immediate feedback to the fan speed
      // yea yea yea i realize i'm computing fan speed but the point
      // is how to give immediate feedback
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from  ESP
          document.getElementById("kat").innerHTML=this.responseText;
        }
      }
      // this syntax is really weird the ? is a delimiter
      // first arg UPDATE_SLIDER is use in the .on method
      // server.on("/UPDATE_SLIDER", UpdateSlider);
      // then the second arg VALUE is use in the processing function
      // String t_state = server.arg("VALUE");
      // then + the controls value property
      xhttp.open("PUT", "UPDATE_KAT?KAT="+value, true);
      xhttp.send();
    }

    function Aktualizuj_PWM(wyp, silnik){
      var xhttp = new XMLHttpRequest();

      if(silnik==1){
        xhttp.onreadystatechange = function() {

        if (this.readyState == 4 && this.status == 200)         
          document.getElementById("PWM_1").innerHTML=this.responseText;
        }
        document.getElementById("Pasek_PWM_oba").value = 1000;
        xhttp.open("PUT", "AKTUALIZUJ_PWM_1?PWM1="+wyp, true);
        xhttp.send();
        return;
             
    }
      else if (silnik==2){
        xhttp.onreadystatechange = function() {

        if (this.readyState == 4 && this.status == 200)         
           document.getElementById("PWM_2").innerHTML=this.responseText;
        
          }
        document.getElementById("Pasek_PWM_oba").value = 1000;
        xhttp.open("PUT", "AKTUALIZUJ_PWM_2?PWM2="+wyp, true);
        xhttp.send();
        return;         

        }
      else if (silnik ==3){
        xhttp.onreadystatechange = function() {

        if (this.readyState == 4 && this.status == 200)         
          document.getElementById("PWM_oba").innerHTML=this.responseText;
          }
        
        document.getElementById("Pasek_PWM_1").value = 1000;
        document.getElementById("Pasek_PWM_2").value = 1000;
        xhttp.open("PUT", "AKTUALIZUJ_PWM_OBA?PWM_oba="+wyp, true);
        xhttp.send();
        return;


        } 
      }


    function Pasek_PID(id, wartosc){
      var xhttp = new XMLHttpRequest();

      xhttp.onreadystatechange = function() {

        if (this.readyState == 4 && this.status == 200)         
            document.getElementById(id + "_war").innerHTML=this.responseText;
            }
        xhttp.open("PUT", "AKTUALIZUJ_PID?ZMIANA_PID="+id+wartosc, true);
        xhttp.send();            
    }

    // function to handle the response from the ESP
    function response(){
      var message;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      xmlResponse=xmlHttp.responseXML;

      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();

      xmldoc = xmlResponse.getElementsByTagName("X1"); //bits for A0
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("osx").innerHTML = message;

      xmldoc = xmlResponse.getElementsByTagName("Y1");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("osy").innerHTML = message;
     }
  
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",20);
    }
  
  
  </script>

</html>

)=====";