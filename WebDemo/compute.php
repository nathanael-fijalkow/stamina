<?php
$iid = session_id();
		
$dir = $iid."@".$_SERVER['REMOTE_ADDR'];

$finput="automaton.".$dir.".acme";
$foutput="computation.".$dir.".log";
$input=$finput;
$output=$foutput;
$automaton = "automaton.".$dir.".acme.dot";

$bin = realpath("WebDemo");
    $action = $_POST['action'];
    $problem = $_POST['problem'];
    if($problem == "smc" || $problem == "smp") $problem = "monoid";
    
    switch($action)
      {
      case 'solve':

	unlink($output);
	unlink($automaton);

	$f = fopen($input,"w+");
	
	$o = fopen($output,"w+");
	if(!file_exists($bin) || !is_file($bin)) {
	    $msg = "Binary not found"; fwrite($o,$msg); echo $msg; break;
	} else if(!is_executable($bin))  {
            if(chmod($bin, 0755))  {
                $msg = "Failed to change permissions of binary file and make it executable"; fwrite($o,$msg); echo $msg;  break;
              }
	  } else if($f === FALSE)  {
	    $msg = "Failed to create input automaton file";  fwrite($o,$msg);  echo $msg; break;
	  } else if($aut["initial"] == '')  {
	    $msg = "This automaton has no initial state.";  fwrite($o,$msg); break;
	  } else if($aut["final"] == '') {
	    $msg = "This automaton has no final state."; fwrite($o,$msg); break;
	  } else  {
	    //we send data to the file
	    $type = $aut["type"];
	    $snb = $aut["statesnb"];
	    $lnb = $aut["lettersnb"];
	    fwrite($f,$snb."\n".$type."\n");
	    $c='a';
	    for($i=0; $i < $lnb; $i++)
	      fwrite($f,$c++);

	    fwrite($f,"\n".$aut["initial"]."\n".$aut["final"]."\n\n");

	    $c='a';
	    for($i=0; $i < $lnb; $i++) {
		fwrite($f,$c++."\n");
		fwrite($f,$aut["mats"][$i]);
	      }
	    fclose($f);
	    fwrite($o,'The <a href="'.$input.'"> automaton file</a> and '); 
	    fwrite($o,'the <a href="'.$output.'"> log file</a>'."\n"); 
	    fclose($o);
	    echo "Computation started";	
	    exec("nice ".$bin." ".$problem." ".$finput." >> ".$foutput. " 2>/dev/null  &");
	  }
	break;
	case 'aut_mtime':
		if(file_exists($automaton)) {
			$fp = fopen($automaton, "r");
			$fstat = fstat($fp);
			fclose($fp);
			return $fstat['mtime'];
		} else {
			return "";
		}
		break;
	case 'aut_file':
		if(file_exists($automaton)) {
			return $automaton;
		} else {
			return "";
		}
		break;		
      case 'progress':
	$size = filesize($output);
	$max = 10000;

	//retourne le contenu du fichier de sortie
	$f =fopen($output,"r");
	if($f ===false)
	  {
	    echo "No output yet";
	    break;
	  } 
	$out = "";

	if($size <= $max)
	  {
	while(true)
	  {
	    $chunk  = fgets($f);
	    if($chunk === false)
	      break;
	    echo str_replace(array("\n","E ","O "),array("<br/>","&epsilon;","&omega;"),$chunk);		
	  }
	  }
	else
	  {
	    for($i = 0; $i < 150; $i++)
	      {
	    $chunk  = fgets($f);
	    echo str_replace(array("\n","E ","O "),array("<br/>","&epsilon;","&omega;"),$chunk);	    
	      }

	    echo "<br/>*************************************************<br/>";
	    echo "<br/>The computation is long, output is truncated...";
	    echo "Here is <a href=\"".$foutput."\">the complete log.</a><br/>";
	    echo "<br/>*************************************************<br/>";

	    fseek($f, $size - $max);
	    $chunk  = fgets($f);

	    while(true)
	      {
	    $chunk  = fgets($f);
	    if($chunk === false)
	      break;
	    echo str_replace(array("\n","E ","O "),array("<br/>","&epsilon;","&omega;"),$chunk);		
	      }
	  }
	fclose($f);
	break;
      case 'stop':
	//tue le processus en cours
	break;
      }
?>
