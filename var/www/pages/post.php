<?php
   if(isset($_POST['submit'])) 
   {
      echo("Customer : " . $_POST['name'] . "<br />\\n");
      echo("Placed order: " . $_POST['order'] . "<br />\\n");
      echo("Contact email: " . $_POST['email'] . "<br />\\n");
   }
?>