<link rel="stylesheet" href="http://localhost:8080/Interactive%20Timeline_22/style.css">

<body>


<div class="crew-side">
<?php
//opening the file and saving the contents to a variable
$jsonData = file_get_contents("newfile.txt", true);
//header('Content-Type: application/json');
$passengerData = json_decode( $jsonData, true );

//Make an array with data from $passengerData as '$passenger'
foreach($passengerData as $passenger) {
  ?>

  <!--power of 'id=' = you can assign tag with id for each class, and you can see in inspect element on the web-->
  <div class="passenger" id="<?php echo $passenger['details']['seatnumber']; ?>">
  <?php
  echo "SEAT NUMBER: ";
  echo $passenger['details']['seatnumber'] . "<BR>";

  //write down automatically key(name)(mealtime1, mealtime2, sleeptime1) and value together
  //echo key and value of an array without and with loop
  foreach($passenger['preferences'] as $key => $value) {
    echo $key . ' : ' . $value . "<br>";
  }

  //what is this..? Close div..?
  echo "</div>";
}
?>
</div>
</body>
