String message, XML;


char setwaktu[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<head>
<title>JWSP10 ELEKTRONMART</title>
<style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>

<h1>Ubah Waktu</h1>
<div class='info'>
  <span id='day'></span>/<span id='month'></span>/<span id='year'></span><span> </span>
  <span id='hour'></span>:<span id='minute'></span>:<span id='second'></span><span> </span>
  <span id='temp'></span><span>C</span><br><br>
</div>
<table width='100%'>
  <tr>
    <td>
      <form >
        <h4>Tanggal</h4>  
        <input type='date' name='date' min='2017-03-20'><br><br>
        <input type='submit' value='Ubah Tanggal'> 
      </form>  
    </td>
    <td>
      <form >  
        <h4>Jam</h4>
        <input type='TIME' name='time'><br><br>
        <input type='submit' value='Ubah Jam'> 
      </form>
    </td>
  </tr>
</table>
<br><br>
<h5><a class ='tombol' href="/setjws">Jam Sholat</a></h5>
<h5><a class ='tombol' href="/setinfo">Informasi</a></h5>
<h5><a class ='tombol' href="/setwifi">Wifi</a></h5>
<h5><a class ='tombol' href="/setdisplay">Display</a></h5>
<br><br><br>
<a class ='tombol' href="http://elektronmart.com/">ElektronMart.Com</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/elektronmart">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/elektronmartcom">Tokopedia</a><br>

</body>
</html>

<script>


var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }
 setTimeout('process()',1000);
}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rYear');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('year').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rMonth');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('month').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rDay');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('day').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rHour');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('hour').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rMinute');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('minute').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rSecond');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('second').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rTemp');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('temp').innerHTML=message;

 }
}

</script>

</html>


)=====";


char setjws[] PROGMEM = R"=====(
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<head>
<title>JWSP10 ELEKTRONMART</title>
<style> body { width: 97% ; text-align: center; } td {text-align: center;} input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1,h2,h3,h4 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>Waktu Sholat</h1>

<h4>Iqomah</h4>
<form>
<table width='100%'>
  <tr>
    <td>
      <label for="iqmhs">Subuh</label><br>
      <input id="iqmhs" placeholder="12" maxlength="2" size="3"/>
    </td>
    <td>
      <label for="iqmhd">Dzuhur</label><br>
      <input id="iqmhd" placeholder="8" maxlength="2" size="3"/>
    </td>
    <td>
      <label for="iqmha">Ashar</label><br>
      <input id="iqmha" placeholder="6" maxlength="2" size="3"/>
    </td>
  </tr>
  <tr>
    <td>
      <label for="iqmhm">Maghrib</label><br>
      <input id="iqmhm" placeholder="5" maxlength="2" size="3"/>
    </td>
    <td>
      <label for="iqmhi">Isya</label><br>
      <input id="iqmhi" placeholder="5" maxlength="2" size="3"/>
    </td>
    <td>
      <label for="durasiadzan">Adzan</label><br>
      <input id="durasiadzan" placeholder="2" maxlength="2" size="3"/>
    </td>
  </tr>
</table>
<table width='100%'>
  <tr>
    <td>
      <label for="latitude">Latitude</label><br>
      <input id="latitude" placeholder="-6.165010" size="9"/>
    </td>
    <td>
      <label for="longitude">Longitude</label><br>
      <input id="longitude" placeholder="106.608892" size="9"/>
    </td>    
  </tr>
  <tr>
    <td>
      <label for="zonawaktu">Zona Waktu</label><br>
      <input id="zonawaktu" placeholder="7" size="3"/>
    </td>
    <td>
      <label for="ihti">Ihtiyati</label><br>
      <input id="ihti" placeholder="2" maxlength="2" size="3"/>
    </td>
  </tr>
</table>
<br>
<div>
  <button onClick="setJws()"> Simpan </button>
</div>
</form>
<br><br>

<h5><a class ='tombol' href="/">Kembali</a></h5>
<h5><a class ='tombol' href="/setinfo">Informasi</a></h5>
<h5><a class ='tombol' href="/setwifi">Wifi</a></h5>
<h5><a class ='tombol' href="/setdisplay">Display</a></h5>
<br><br><br>
<a class ='tombol' href="http://elektronmart.com/">ElektronMart.Com</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/elektronmart">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/elektronmartcom">Tokopedia</a><br>

</body>
<script>


var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }

}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rIqmhs');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhs').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rIqmhd');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhd').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rIqmha');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmha').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rIqmhm');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhm').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rIqmhi');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhi').value=message;

    xmldoc = xmlResponse.getElementsByTagName('rDurasiAdzan');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('durasiadzan').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rIhti');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('ihti').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rLatitude');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('latitude').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rLongitude');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('longitude').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rZonaWaktu');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('zonawaktu').value=message;

 }
}


function setJws() {
  console.log("tombol telah ditekan!");

  var iqmhs = document.getElementById("iqmhs").value;
  var iqmhd = document.getElementById("iqmhd").value;
  var iqmha = document.getElementById("iqmha").value;
  var iqmhm = document.getElementById("iqmhm").value;
  var iqmhi = document.getElementById("iqmhi").value;
  var durasiadzan = document.getElementById("durasiadzan").value;
  var ihti = document.getElementById("ihti").value;
  var latitude = document.getElementById("latitude").value;
  var longitude = document.getElementById("longitude").value;
  var zonawaktu = document.getElementById("zonawaktu").value;
  var datajws = {iqmhs:iqmhs, iqmhd:iqmhd, iqmha:iqmha, iqmhm:iqmhm, iqmhi:iqmhi, durasiadzan:durasiadzan, ihti:ihti, latitude:latitude, longitude:longitude, zonawaktu:zonawaktu};

  var xhr = new XMLHttpRequest();
  var url = "/settingjws";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(datajws));
};
</script>
</html>
)=====";



char setinfo[] PROGMEM = R"=====(
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<head>
<title>JWSP10 ELEKTRONMART</title>
<style> body { width: 97% ; text-align: center; } td {text-align: center;} input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1,h2,h3,h4 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>Informasi</h1>

<form>
<div>
  <label for="nama">Nama</label>
  <input id="nama" placeholder="Nama"/>
</div>
<div>
  <label for="info1">Info Satu</label>
  <input id="info1" placeholder="Ini adalah isi dari Info Satu"/>
</div>
<div>
  <label for="info2">Info Dua</label>
  <input id="info2" placeholder="Ini adalah isi dari Info Dua"/>
</div>
<br>
<div>
  <button onClick="setInfo()"> Simpan </button>
</div>
</form>
<br><br>

<h5><a class ='tombol' href="/">Kembali</a></h5>
<h5><a class ='tombol' href="/setjws">Jam Sholat</a></h5>
<h5><a class ='tombol' href="/setwifi">Wifi</a></h5>
<h5><a class ='tombol' href="/setdisplay">Display</a></h5>
<br><br><br>
<a class ='tombol' href="http://elektronmart.com/">ElektronMart.Com</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/elektronmart">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/elektronmartcom">Tokopedia</a><br>

</body>
<script>


var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }

}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rNama');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('nama').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rInfo1');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('info1').value=message;

   xmldoc = xmlResponse.getElementsByTagName('rInfo2');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('info2').value=message;

 }
}


function setInfo() {
  console.log("tombol telah ditekan!");

  var nama = document.getElementById("nama").value;
  var info1 = document.getElementById("info1").value;
  var info2 = document.getElementById("info2").value;
  var datainfo = {nama:nama, info1:info1, info2:info2};

  var xhr = new XMLHttpRequest();
  var url = "/settinginfo";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(datainfo));
};
</script>
</html>
)=====";



char setwifi[] PROGMEM = R"=====(
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<head>
<title>JWSP10 ELEKTRONMART</title>
<style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>Ubah Wifi</h1>
<form>
  <div>
    <label for="wifissid">Wifi SSID</label>
    <input id="wifissid"/>
  </div>
  <div>
    <label for="wifipassword">Wifi Password</label>
    <input type="wifipassword" id="wifipassword"/>
  </div>
  <div>
    <button onClick="setWifi()"> Simpan & Restart </button>
  </div>
</form>
<br><br>
<h5><a class ='tombol' href="/">Kembali</a></h5>
<h5><a class ='tombol' href="/setjws">Jam Sholat</a></h5>
<h5><a class ='tombol' href="/setinfo">Informasi</a></h5>
<h5><a class ='tombol' href="/setdisplay">Display</a></h5>
<br><br><br>
<a class ='tombol' href="http://elektronmart.com/">ElektronMart.Com</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/elektronmart">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/elektronmartcom">Tokopedia</a><br>

</body>

<script>

var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }

}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rWifissid');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('wifissid').value=message;

 }
}

function setWifi() {
  console.log("tombol telah ditekan!");

  var wifissid = document.getElementById("wifissid").value;
  var wifipassword = document.getElementById("wifipassword").value;
  var data = {wifissid:wifissid, wifipassword:wifipassword};

  var xhr = new XMLHttpRequest();
  var url = "/settingwifi";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(data));
};
</script>
</html>
)=====";



char setdisplay[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<head>
  <title>JWSP10 ELEKTRONMART</title>
  <style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>Display</h1>
<form id='data_form' name='frmText'>
  <label>Redup<input id="cerah" type='range' name='Cerah' min='18' max='255'>Cerah
  <br>
</form>
<br>
<button onClick="setDisp()"> Simpan </button><span> </span>
<br><br>
<h5><a class ='tombol' href="/">Kembali</a></h5>
<h5><a class ='tombol' href="/setjws">Jam Sholat</a></h5>
<h5><a class ='tombol' href="/setinfo">Informasi</a></h5>
<h5><a class ='tombol' href="/setwifi">Wifi</a></h5>
<br><br>
<a class ='tombol' href="http://elektronmart.com/">ElektronMart.Com</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/elektronmart">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/elektronmartcom">Tokopedia</a><br>

</body>

<script>

function setDisp() {

  console.log("tombol telah ditekan!");
  
  var cerah = document.getElementById("cerah").value;
  var datadisp = {cerah:cerah};

  var xhr = new XMLHttpRequest();
  var url = "/settingdisp";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(datadisp));
  
}

var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }

}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rCerah');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('cerah').value=message;

 }
}


</script>

</html>


)=====";
