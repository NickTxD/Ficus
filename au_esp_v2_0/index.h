const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<div">
<h1>PROJECT FICUS</h1>
	<span id="Data">Wait for data</span><br>
</div>
<script>
setInterval(function() {
  getData();
}, 1000);
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Data").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "getData", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";