<link rel="stylesheet" href="http://localhost:8080/Interactive%20Timeline_22/style.css">


<body>
<button onclick="goBack()">Go Back</button>

<script>
function goBack() {
    window.history.back();
}
</script>

<?php
echo "<font size='18'>";
echo '<br>';

if (isset($_POST["seatnumber"]) && isset($_POST["mealtime1"]) && isset($_POST["mealtime2"]) && isset($_POST["sleeptime1"])) {

  echo 'Thank you for your information';

  //opening the file and saving the contents to a variable
  $jsonData = file_get_contents("newfile.txt", true);

  //turning json data into PHP array/object
  $passengerData = json_decode( $jsonData, true );

  $passengerData[ $_REQUEST[ "seatnumber" ] ] = array(
    "details" => array(
      "seatnumber" => $_REQUEST["seatnumber"]
    ),
    "preferences" => array(
      "mealtime1" => $_REQUEST["mealtime1"],
      "mealtime2" => $_REQUEST["mealtime2"],
      "sleeptime1" => $_REQUEST["sleeptime1"]
    )
  );

  //Objects in PHP can be converted into JSON by using the PHP function json_encode()
  $jsonData = json_encode($passengerData);
  file_put_contents("newfile.txt", $jsonData);
}
?>
</body>
