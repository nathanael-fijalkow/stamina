<?php
session_start();

if(isset($_POST['action']))
{
		include 'compute.php';
}
else
  {
 	include('content.html');
  }
    ?>
