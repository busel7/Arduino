// -----------------------------
// HALAMAN WEB UNTUK SETTING JWS

char setwaktu[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<meta name='viewport' content='width=device-width, initial-scale=1'>

<head>
<title>JWSP10 ELEKTRONMART</title>
<style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>

<body onload='updateData()'>

<h1>JWS LED P10 HUB12</h1>
<h4><a href="http://elektronmart.com">ElektronMart.Com</a></h4>

<div class='info'>
  <span id='day'></span>/<span id='month'></span>/<span id='year'></span><span> </span>
  <span id='hour'></span>:<span id='minute'></span>:<span id='second'></span><span> </span>
  <span id='temp'></span><span>C</span><br><br>
</div>

<h2>Setting JWS</h2>
<h3>Iqomah</h3>

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
  <tr>
    <td colspan="2">
      <label for="hilal">Hilal</label><br>
      <input id="hilal" placeholder="2" maxlength="2" size="3"/>
    </td>
  </tr>
</table>
<br>
<div>
  <button onClick="setJws()"> Simpan </button>
</div>
</form>

<br><br><br>

<h3>Informasi</h3>
<form>
  <div>
    <label for="namamasjid">Nama Masjid</label><br>
    <textarea rows="3" id="namamasjid" placeholder="Masjid Al Kautsar"/></textarea>
  </div>
  <div>
    <br>
    <button onClick="setJws()"> Simpan </button>
  </div>
</form>

<br><br>
<hr/>
<br><br>

<h2>Ubah Waktu</h2>

<table width='100%'>
  <tr>
    <td>
      <form>
        <h4>Tanggal</h4>  
        <input type='date' name='date' min='2019-01-01'><br><br>
        <input type='submit' value='Ubah Tanggal'> 
      </form>  
    </td>
    <td>
      <form>  
        <h4>Jam</h4>
        <input type='TIME' name='time'><br><br>
        <input type='submit' value='Ubah Jam'> 
      </form>
    </td>
  </tr>
</table>
<br><br><br>
<hr/>
<br><br>
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

function updateData() {
  prosesdatajws();
  proseswaktu();
}

function proseswaktu(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xmlwaktu',true);
   xmlHttp.onreadystatechange=handleServerResponseWaktu;
   xmlHttp.send(null);
 }
 setTimeout('proseswaktu()',1000);
}

function prosesdatajws(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xmldatajws',true);
   xmlHttp.onreadystatechange=handleServerResponseDataJWS;
   xmlHttp.send(null);
 }
}


function handleServerResponseWaktu(){
  
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('Tahun');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('year').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('Bulan');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('month').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('Tanggal');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('day').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('Jam');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('hour').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('Menit');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('minute').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('Detik');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('second').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('Suhu');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('temp').innerHTML=message;
 }
 
}


function handleServerResponseDataJWS(){
  
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('IqomahSubuh');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhs').value=message;

   xmldoc = xmlResponse.getElementsByTagName('IqomahDzuhur');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhd').value=message;

   xmldoc = xmlResponse.getElementsByTagName('IqomahAshar');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmha').value=message;

   xmldoc = xmlResponse.getElementsByTagName('IqomahMaghrib');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhm').value=message;

   xmldoc = xmlResponse.getElementsByTagName('IqomahIsya');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('iqmhi').value=message;

   xmldoc = xmlResponse.getElementsByTagName('DurasiAdzan');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('durasiadzan').value=message;

   xmldoc = xmlResponse.getElementsByTagName('Ihtiyati');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('ihti').value=message;

   xmldoc = xmlResponse.getElementsByTagName('Latitude');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('latitude').value=message;

   xmldoc = xmlResponse.getElementsByTagName('Longitude');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('longitude').value=message;

   xmldoc = xmlResponse.getElementsByTagName('ZonaWaktu');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('zonawaktu').value=message;

   xmldoc = xmlResponse.getElementsByTagName('Hilal');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('hilal').value=message;

   xmldoc = xmlResponse.getElementsByTagName('NamaMasjid');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('namamasjid').value=message;
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
  var hilal = document.getElementById("hilal").value;
  var namamasjid = document.getElementById("namamasjid").value;
  
  var datajws = {iqmhs:iqmhs, iqmhd:iqmhd, iqmha:iqmha, iqmhm:iqmhm, iqmhi:iqmhi, durasiadzan:durasiadzan, ihti:ihti, latitude:latitude, longitude:longitude, zonawaktu:zonawaktu, hilal:hilal, namamasjid:namamasjid};
  
  var xhr = new XMLHttpRequest();
  var url = "/simpandatajws";
  
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
