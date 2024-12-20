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

  datalist {
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  writing-mode:sideways-rl; 
  font-size: small;
  width: 90%;
}


option {
  padding: 0;
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
            <div class = "row">
              <div class="category">Odczyty czujnika MPU6050</div>
              <div class = "column" style = "width: 40%;">             
              <br>
              <div class="bodytext">Obrót wokół osi X: <span id="osx">123</span>
                 <br><br>
                 
                  Obrót wokół osi Y: <span id="osy">123</span>   
                  <br><br>
                 
             
            </div>
          </div> 
        
            <div class = "column" style = "width: 30%;">
              
              <br>
              <div class="bodytext">Szybkość: <span id="xkalmr_wysw">123</span>
                 <br><br>
                 Szybkość: <span id="ykalmr_wysw">123</span>
                  <br>

             </div>
            </div>

            <div class = "column" style = "width: 30%;">
              
              <br>
              <div class="bodytext">
                Niepewność: <span id="xkalmu_wysw">123</span> 
                
                <br><br>
                 Niepewność: <span id="ykalmu_wysw">123</span>  
                 <br><br>

             </div>
            </div>
          
        </div>
        <div class = "bodytext">Wysokość nad podstawą: <span id="wys">123</span></div>
        <br><br>

      <!-- >
        <button type="button" class="btn" id="kalibracja" onclick="Kalibracja()">Kalibracja czujnika</button>
        <div id="myProgress">
          <div id="myBar"></div>     
        </div>
        <br>
        <!-->
    <div class ="row">
      <div class="column">
        <div class="bodytext">
          Odczyt obrotu wokół osi X: <span id="czysty_x">123</span>
        </div>
      </div>

      <div class = "column">
        <div class = "bodytext">
          Odczyt obrotu wokół osi Y: <span id="czysty_y">123</span>
        </div>
      </div>

    </div>
    <br>
    <button class="btn" id = "pobierz_dane" onclick = "pobierzDane()">Pobierz dane</button>
    <button class="btn" id = "pobierz_dane" onclick = "czyscDane()">Czyść dane</button>
    <textarea style = "display: none" id = "dane_pudlo" style="white-space: pre-line;">XKALM, YKALM, XCZYST, YCZYST, KZAD, WZAD, WYS, CZAS

    </textarea>

    <div class="category">Wartości zadane</div>
    <br>
    <div class="bodytext">Zadana wysokość: <span id="wysokosc">5</span> cm</div>
    <br>
    <input type="range" class="fanrpmslider" list ="war_suwak_wys" min="5" max="40"  value = "5" width = "0%" oninput="UpdateWysokosc(this.value)"/>       
    <datalist id = "war_suwak_wys" >
      <option value=40 label=40></option>
      <option value=39 label=39></option>
      <option value=38 label=38></option>
      <option value=37 label=37></option>
      <option value=36 label=36></option>
      <option value=35 label=35></option>
      <option value=34 label=34></option>
      <option value=33 label=33></option>
      <option value=32 label=32></option>
      <option value=31 label=31></option>
      <option value=30 label=30></option>
      <option value=29 label=29></option>
      <option value=28 label=28></option>
      <option value=27 label=27></option>
      <option value=26 label=26></option>
      <option value=25 label=25></option>
      <option value=24 label=24></option>
      <option value=23 label=23></option>
      <option value=22 label=22></option>
      <option value=21 label=21></option>
      <option value=20 label=20></option>
      <option value=19 label=19></option>
      <option value=18 label=18></option>
      <option value=17 label=17></option>
      <option value=16 label=16></option>
      <option value=15 label=15></option>
      <option value=14 label=14></option>
      <option value=13 label=13></option>
      <option value=12 label=12></option>
      <option value=11 label=11></option>
      <option value=10 label=10></option>
      <option value=9 label=9></option>
      <option value=8 label=8></option>
      <option value=7 label=7></option>
      <option value=6 label=6></option>
      <option value=5 label=5></option>
    </datalist>
 
    <br>

    <div class="bodytext">Zadany kąt: <span id="kat">0</span> stopni</div>
    <br>
    <input type="range" class="fanrpmslider" list="war_suwak_kat" min="-25" max="25"  value = "0" width = "0%" oninput="UpdateKat(this.value)"/>
    <br>
    <br>

    <datalist id ="war_suwak_kat">
      <option value=-25 label=-25></option>
      <option value=-24 label=-24></option>
      <option value=-23 label=-23></option>
      <option value=-22 label=-22></option>
      <option value=-21 label=-21></option>
      <option value=-20 label=-20></option>
      <option value=-19 label=-19></option>
      <option value=-18 label=-18></option>
      <option value=-17 label=-17></option>
      <option value=-16 label=-16></option>
      <option value=-15 label=-15></option>
      <option value=-14 label=-14></option>
      <option value=-13 label=-13></option>
      <option value=-12 label=-12></option>
      <option value=-11 label=-11></option>
      <option value=-10 label=-10></option>
      <option value=-9 label=-9></option>
      <option value=-8 label=-8></option>
      <option value=-7 label=-7></option>
      <option value=-6 label=-6></option>
      <option value=-5 label=-5></option>
      <option value=-4 label=-4></option>
      <option value=-3 label=-3></option>
      <option value=-2 label=-2></option>
      <option value=-1 label=-1></option>
      <option value=0 label=0></option>
      <option value=1 label=1></option>
      <option value=2 label=2></option>
      <option value=3 label=3></option>
      <option value=4 label=4></option>
      <option value=5 label=5></option>
      <option value=6 label=6></option>
      <option value=7 label=7></option>
      <option value=8 label=8></option>
      <option value=9 label=9></option>
     <option value=10 label=10></option>
      <option value=11 label=11></option>
      <option value=12 label=12></option>
      <option value=13 label=13></option>
      <option value=14 label=14></option>
      <option value=15 label=15></option>
      <option value=16 label=16></option>
      <option value=17 label=17></option>
      <option value=18 label=18></option>
      <option value=19 label=19></option>
      <option value=20 label=20></option>
      <option value=21 label=21></option>
      <option value=22 label=22></option>
      <option value=23 label=23></option>
      <option value=24 label=24></option>
      <option value=25 label=25></option>

    </datalist>

 
    
    <div class="category">Ręczne sterowanie wypełnieniem PWM</div>
    <br>
    <div class="bodytext">Zadane wypełnienie silnika 1: <span id="PWM_1">1000</span> &mu;s</div>
    <br>
    <input type="range" class="fanrpmslider" min="1000" max="2000" value = "1000" width = "0%" id ="Pasek_PWM_1" oninput="Aktualizuj_PWM(this.value, 1)"/>
    <br>
    <div class="bodytext">Zadane wypełnienie silnika 2: <span id="PWM_2">1000</span> &mu;s</div>
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
              <div class ="bodytext"> Współczynnik k<sub>p</sub>: <span id="Pasek_wys_kp_war">0.5</span> <sup>%</sup>&frasl;<sub>°</sub>  </div> 
              <input type="range" class="fanrpmslider" min="0.01" max="10" value = "0.01" width = "0%" step = "0.002" id = "Pasek_wys_kp" oninput="Pasek_PID(this.id, this.value)"/>
              <br>

              <br>
              <div class ="bodytext"> Współczynnik k<sub>i</sub>:  <span id="Pasek_wys_ki_war">0</span> <sup>%</sup>&frasl;<sub>°s</sub> </div> 
              <input type="range" class="fanrpmslider" min="0" max="5" value = "0" width = "0%" step = "0.002" id = "Pasek_wys_ki" oninput="Pasek_PID(this.id, this.value)"/>
              <br>

              <br>
              <div class ="bodytext"> Współczynnik k<sub>d</sub>: <span id="Pasek_wys_kd_war">0</span> <sup>%s</sup>&frasl;<sub>°</sub> </div> 
              <input type="range" class="fanrpmslider" min="0" max="5" value = "0" width = "0%" step = "0.002" id = "Pasek_wys_kd" oninput="Pasek_PID(this.id, this.value)"/>
              <br>

          </div>
        <div class = "column">
          <div class = "bodytext" style = "font-size: 30px;">Regulacja kąta</div>
          <br>
          <br>
          <div class ="bodytext"> Współczynnik k<sub>p</sub>: <span id="Pasek_kat_kp_war">0.5</span><sup>%</sup>&frasl;<sub>°</sub> </div> 
          <input type="range" class="fanrpmslider" min="0.01" max="10" value = "0.01" width = "0%" step = "0.002" id = "Pasek_kat_kp" oninput="Pasek_PID(this.id, this.value)"/>
          <br>

          <br>
          <div class ="bodytext"> Współczynnik k<sub>i</sub>:  <span id="Pasek_kat_ki_war">0</span> <sup>%</sup>&frasl;<sub>°s</sub>  </div> 
          <input type="range" class="fanrpmslider" min="0" max="5" value = "0" width = "0%" step = "0.002" id = "Pasek_kat_ki" oninput="Pasek_PID(this.id, this.value)"/>
          <br>

          <br>
          <div class ="bodytext"> Współczynnik k<sub>d</sub>: <span id="Pasek_kat_kd_war">0</span> <sup>%s</sup>&frasl;<sub>°</sub> </div> 
          <input type="range" class="fanrpmslider" min="0" max="5" value = "0" width = "0%" step = "0.002" id = "Pasek_kat_kd" oninput="Pasek_PID(this.id, this.value)"/>
          <br>
        </div>
        <div class = "row">
          <div class = "bodytext">
          <br>
          <br>
          <div class="bodytext">współczynnik mocy <span id="wsp_mocy_tekst">30</span> %</div>
          <input type="range" class="fanrpmslider" min="0" max="100" value = "30" width = "0%"  id = "Pasek_wsp_moc" oninput="Pasek_moc(this.value)"/>
          <br>
          <br>
          <br>
        <input type="checkbox" id ="sterowanie" tryb="ręczne" oninput="Sterowanie_przycisk(this.id)"  ></input>Sterowanie automatyczne
        <button type="button" id ="button_stop" class="btn" onclick = "btn_stop()" style = "height: 60px; width: 120px;"> STOP</button>
        <br>
        <br>
        <input type = "radio" id = "radio_wys" name = "wybor_ster" value="w" oninput="Sterowanie_przycisk(this.id)"> <label for="radio_wys" name = "wybor_ster">Sterowanie wysokością</label>
        <input type = "radio" id = "radio_kat" name = "wybor_ster" value="k"  checked="true" oninput="Sterowanie_przycisk(this.id)"> <label for="radio_kat" name = "wybor_ster" >Sterowanie kątem</label>
        <br>
        <input type = "radio" id = "radio_oba" name = "wybor_ster" value="o" oninput="Sterowanie_przycisk(this.id)"> <label for="radio_oba"  name = "wybor_ster">Sterowanie wysokością i kątem</label>
        
        <br>
        <br>
        Aktualne sterowanie: <span id="Sterowanie_tekst" >Sterowanie ręczne</span>
        
        <br>
        <div class="bodytext">Wypełnienie silnika 1: <span id="pwm1_wysw"></span> &mu;s</div>
        <br>
        <div class="bodytext">Wypełnienie silnika 2: <span id="pwm2_wysw"></span> &mu;s</div>
      </div>
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

    function Pasek_moc(moc){
      var xhttp = new XMLHttpRequest(); 

      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("wsp_mocy_tekst").innerHTML = this.responseText;
        }
      }

      xhttp.open("PUT", "PASEK_MOC?MOC="+moc, false);
      xhttp.send(); 
      

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
            document.getElementById(id + "_war").innerHTML=(parseFloat(this.responseText)*50).toFixed(1);
            document.getElementById(id + "_war").innerHTML = document.getElementById(id + "_war").innerHTML.replace(".", ",");
            }
        xhttp.open("PUT", "AKTUALIZUJ_PID?ZMIANA_PID="+id+(wartosc*100), true);
        xhttp.send();            
    }


    function btn_stop(){
      Aktualizuj_PWM(1000, 3); 
      if(document.getElementById("sterowanie").getAttribute("tryb")=="auto")
        document.getElementById("sterowanie").click();

    }

    function pobierzDane(){
      var dt = new Date();     
      const link = document.createElement("a");
      const content = document.getElementById("dane_pudlo").textContent;     
      const file = new Blob([content], { type: 'text/plain' });
      link.href = URL.createObjectURL(file);
      link.download = "Dane_czujnik" + dt.toLocaleTimeString()  +".csv";
      link.click();
      URL.revokeObjectURL(link.href);
      document.getElementById("dane_pudlo").textContent = "XKALM, YKALM, XCZYST, YCZYST, KZAD, WZAD, WYS, CZAS \n";

    }

    function czyscDane(){
      document.getElementById("dane_pudlo").textContent = "XKALM, YKALM, XCZYST, YCZYST, KZAD, WZAD, WYS, CZAS \n";
    }

    function Sterowanie_przycisk(id){     
      var pudlo = document.getElementById("sterowanie");
      var ster = String(document.querySelector('input[name="wybor_ster"]:checked').value);
      var tryb_ster = "aa";

      if(id != "sterowanie")
    {
      if(pudlo.getAttribute("tryb") == "ręczne"){

        tryb_ster = "r";
        document.getElementById("Sterowanie_tekst").textContent = "Sterowanie ręczne";
      }
      else if(pudlo.getAttribute("tryb") == "auto"){
        tryb_ster = "a";
        document.getElementById("Sterowanie_tekst").textContent = "Sterowanie automatyczne";
      }
    }

      
      
      if(id =="sterowanie"){

      if(pudlo.getAttribute("tryb") == "ręczne"){
        tryb_ster ="a";
        pudlo.setAttribute("tryb", "auto");
        document.getElementById("Sterowanie_tekst").textContent = "Sterowanie automatyczne";


          }
      
      else if(pudlo.getAttribute("tryb") == "auto"){
        tryb_ster="r";
        pudlo.setAttribute("tryb", "ręczne");
        document.getElementById("Sterowanie_tekst").textContent = "Sterowanie ręczne";
          }
        }

      
      var xhttp = new XMLHttpRequest();
       xhttp.onreadystatechange = function() {
         if (this.readyState == 4 && this.status == 200 && pudlo.getAttribute("tryb") == "auto" ) {
           // update the web based on reply from  ESP
           var tekst = " ";
           if(this.responseText =="o")
            tekst = " wysokością i kątem";
          else if(this.responseText =="k")
            tekst = " kątem";
          else if(this.responseText =="w")
            tekst = " wysokością";
           document.getElementById("Sterowanie_tekst").innerHTML+= tekst;
         }
       }
      xhttp.open("PUT", "UPDATE_STER?STER="+ster+tryb_ster, true);
      xhttp.send();

      return;
    }
    

    // function to handle the response from the ESP
    function response(){
      var message;
      var xmlResponse;
      var xmldoc;
      var len=0;
      var wiersz = "\n";
      var dt = new Date();
      xmlResponse=xmlHttp.responseXML;

      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();

      xmldoc = xmlResponse.getElementsByTagName("X1"); //bits for A0
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");

      message = message.substring(0, message.length-3);
      document.getElementById("osx").innerHTML = message;
      message = message.replace(",", ".");
      wiersz += (message + ", ");
      

      xmldoc = xmlResponse.getElementsByTagName("Y1");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("osy").innerHTML = message;
      message = message.replace(",", ".");
      wiersz += (message + ", ");
      

      xmldoc = xmlResponse.getElementsByTagName("W1");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      
      document.getElementById("wys").innerHTML = message;

      xmldoc = xmlResponse.getElementsByTagName("PWM1");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("pwm1_wysw").innerHTML = message;

      xmldoc = xmlResponse.getElementsByTagName("PWM2");
      message = xmldoc[0].firstChild.nodeValue;
      
      document.getElementById("pwm2_wysw").innerHTML = message;


      /*
      xmldoc = xmlResponse.getElementsByTagName("XKALMR");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("xkalmr_wysw").innerHTML = message; 
        
      
      xmldoc = xmlResponse.getElementsByTagName("XKALMU");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("xkalmu_wysw").innerHTML = message; 
      */
      xmldoc = xmlResponse.getElementsByTagName("YKALMR");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("ykalmr_wysw").innerHTML = message; 

      xmldoc = xmlResponse.getElementsByTagName("YKALMU");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("ykalmu_wysw").innerHTML = message; 
      

      xmldoc = xmlResponse.getElementsByTagName("XCZYST");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("czysty_x").innerHTML = message;
      message = message.replace(",", ".");
      wiersz += (message + ", ");
      

      xmldoc = xmlResponse.getElementsByTagName("YCZYST");
      message = xmldoc[0].firstChild.nodeValue;
      message = String(message).replace(".", ",");
      message = message.substring(0, message.length-3);
      document.getElementById("czysty_y").innerHTML = message;
      message = message.replace(",", ".");
      wiersz += (message + ", ");
      


      wiersz+= (document.getElementById("kat").textContent + ", ");
      wiersz+= (document.getElementById("wysokosc").textContent + ", ");
      wiersz+= (document.getElementById("wys").textContent.replace(",", ".") + ", ") ;
      wiersz+=String(dt.getTime()).substring(4);

      document.getElementById("dane_pudlo").textContent+= wiersz;

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
        setTimeout("process()",10);
    }
  
  
  </script>

</html>



)=====";
