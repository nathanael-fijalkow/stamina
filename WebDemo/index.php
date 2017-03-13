<?php
session_start();
$iid = session_id();
		
$dir = $iid."@".$_SERVER['REMOTE_ADDR'];

$finput="automaton.".$dir.".acme";
$foutput="computation.".$dir.".log";
//$input="demo/".$finput;
//$output="demo/".$foutput;
//$bin="webdemo";
$input=$finput;
$output=$foutput;
$bin = realpath("WebDemo");

if(isset($_POST['action']))
  {
    $action = $_POST['action'];
    switch($action)
      {
      case 'solve':
	$aut=$_POST['automaton'];

	//starts a new computation
	//we create the computation dir
	//		     if(!is_dir($dir))
	//		         mkdir($dir,"0777",true);
	//we create the input file


	$f = fopen($input,"w+");
	$o = fopen($output,"w+");
	if(!file_exists($bin) || !is_file($bin))
	  {
	    $msg = "Binary not found";
	    fwrite($o,$msg);
	    echo $msg;
	    break;
	  }
	if(!is_executable($bin))
          {
            if(chmod($bin, 0755))
              {
                $msg = "Can't make binary executable";
                fwrite($o,$msg);
                echo $msg;
                break;
              }
	  }
	if($f === FALSE)
	  {
	    $msg = "Failed to create input automaton file";
	    fwrite($o,$msg);
	    echo $msg;
	    break;
	  }
	else if($aut["initial"] == '')
	  {
	    $msg = "This automaton has no initial state.";
	    fwrite($o,$msg);
	    break;
	  }
	else if($aut["final"] == '')
	  {
	    $msg = "This automaton has no final state.";
	    fwrite($o,$msg);
	    break;
	  }
	else
	  {

	    //we send data to the file
	    $type = $aut["type"];
	    $snb = $aut["statesnb"];
	    $lnb = $aut["lettersnb"];
	    fwrite($f,$snb."\n".$type."\n");
	    $c='a';
	    for($i=0; $i < $lnb; $i++)
	      fwrite($f,$c++);
	    fwrite($f,"\n".$aut["initial"]);
	    fwrite($f,"\n".$aut["final"]);
	    fwrite($f,"\n");
	    $c='a';
	    for($i=0; $i < $lnb; $i++)
	      {
		fwrite($f,$c++."\n");
		fwrite($f,$aut["mats"][$i]);
	      }
	    fclose($f);
	    fwrite($o,'The <a href="'.$input.'"> automaton file</a> and '); 
	    fwrite($o,'the <a href="'.$output.'"> log file</a>'."\n"); 
	    fclose($o);
	    echo "Computation started";	
	    exec("nice ".$bin." ".$action." ".$finput." >> ".$foutput. " 2>/dev/null  &");
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
	    for($i = 0; $i < 50; $i++)
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
  }
else
  {
    ?>

<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<link rel="stylesheet" type="text/css" href="hugo.css" media="screen" />
<title>Stamina: Stabilisation Monoids IN Automata Theory</title>
<style>
   table, th, td {
 margin:5px;
 }
   body {
   margin: 10% 10% 10% 10%;
   }
</style>
</head>
<body margin="10">
      <script src="jquery-2.2.0.min.js" type="text/javascript"></script>

<div class="avmenu">
<ul>
<li><a href="#Presentation">Presentation</a></li>
<li><a href="https://github.com/nathanael-fijalkow/stamina/">Code</a></li>
<li><a href="#Starheight">Starheight</a></li>
<li><a href="#Demo">Demo</a></li>
<li><a href="#Installation">Installation</a></li>
<li><a href="#Sage">Sage</a></li>


</ul>
</div>
<div class="containeur" id="Contact">
    <center><h1>STAMINA: STabilisation Monoids IN Automata Theory</h1></center>
<br/>
<br/>
<h2 id="Presentation">Presentation</h2>

Stamina is a tool implementing algebraic techniques to solve decision problems from automata theory.
    Most importantly, it is, to the best of our knowledge, the very first implementation of an algorithm solving the
 <a href="http://en.wikipedia.org/wiki/Star_height_problem">starheight problem</a>!
<br/>

    It has been written in C++, by <a href="https://www.cs.ox.ac.uk/people/nathanael.fijalkow/">Nathana&euml;l Fijalkow</a>,
   <a href="http://www.labri.fr/perso/gimbert"/>Hugo Gimbert</a>, 
Edon Kelmendi and <a href="http://www.liafa.univ-paris-diderot.fr/~dkuperbe/">Denis Kuperberg</a>.
The code is available <a href="https://github.com/nathanael-fijalkow/stamina/">on Github</a>.
<br/>

    The core generic algorithm takes as input an automaton and computes its stabilisation monoid,
which is a generalisation of its transition monoid.
<br/>

    Stamina is the successor of <a href="?page=acme">ACME</a>, which also implements the transformation of an automaton into a stabilisation monoid. 
    Thanks to several optimisations, Stamina is much faster and can handle much larger automata,
which is necessary to solve the starheight problem.
<br/>

      <p>Stamina may be used to solve
				     														       these three algorithmic problems:
      </p>
      <ul>
													       
<li> <b>Starheight Problem </b>: compute the starheight of a regular language,
      i.e. the minimal number of nested Kleene stars needed
      for expressing the language with a regular expression.</li>
		       <li> <b>Boundedness problem </b> : decide boundedness of a 
		       regular cost function.</li>
				     <li><b>Value 1 problem </b> : decide whether a leaktight probabilistic automaton has value 1,
				     i.e. whether the automaton accepts some words
				     with probability arbitrarily close to 1.</li>
				     </ul>
				     <p>
				     These three problems reduce to the computation
				     of the stabilization monoid associated with the automaton,
				     which is a challenge since the monoid is exponentially larger than the automaton.
				     The compact data structures used in Stamina, together with optimizations and heuristics,
				     allow this program to handle automata with several hundreds of states.</p>
				     <p>

<hr/>
<h2 id="Support">Support</h2>
<a href="http://perso.crans.org/~genest/stoch.html">
<!--<img src="http://www.logomaker.com/logo-images/52b6422d61306f5e.gif" height="50"></img>-->
</a>
<p>
The developpment of this tool has been supported by the <a href="http://perso.crans.org/~genest/stoch.html">ANR project StochMC</a>.
</p>
<hr/>
<h2 id="Starheight">Starheight computation examples</h2>
<p>Stamina was able to determine the starheight of the following expressions.</p>
<table>
<tr><th>Expression</th><th>Starheight</th><th>Automaton</th><th>Computation Log</th></tr>
<tr><td>(aa)*</td><td>1</td>
<td><a href="Examples/sh_1">sh_1</a></td>
<td><a href="Examples/sh_1.out.txt">sh_1.out</a></td>
</tr>
<tr><td>b*(b*ab*a)*</td><td>2</td>
<td><a href="Examples/sh_2">sh_2</a></td>
<td><a href="Examples/sh_2.out.txt">sh_2.out</a></td>
</tr>
<tr>
<td>(aa(ab)*bb(ab*))*</td><td>2</td>
<td><a href="Examples/sh_3">sh_3</a></td>
<td><a href="Examples/sh_3.out.txt">sh_3.out</a></td>
</tr>
				     <tr><td>(a*b*c)*</td><td>2</td>
<td><a href="Examples/sh_4">sh_4</a></td>
<td><a href="Examples/sh_4.out.txt">sh_4.out</a></td>
</tr>
				     <tr><td>(b*ab*)^6</td><td>1</td>
<td><a href="Examples/sh_5">sh_5</a></td>
<td><a href="Examples/sh_5.out.txt">sh_5.out</a></td>
</tr>
				     <tr><td>(ab + aabb)^*</td><td>1</td>
<td><a href="Examples/sh_6">sh_6</a></td>
<td><a href="Examples/sh_6.out.txt">sh_6.out</a></td>
</tr>
</table>
<hr/>
<h2 id="Demo">Online demo</h2>
<p>The following online demo can be used to solve the bounddness problem for automata with counters
and the value 1 problems for probabilistic automata.

				     <form method="post">
				     <table>
				     <tr>
				     <td>
				     <b>Problem to solve</b>

				     <select class="matchanger" id="problem">
				     <option selected="selected" value="sh">Computing Starheight of a Deterministic Automaton</option>
				     <option value="cl">Checking Limitedness of a MultiCounter Automaton</option>
				     <option value="vo">Checking Value 1 of a Probabilistic Automaton</option>
				     <option value="smc">Computing the Stabilisation Monoid of a MultiCounter Automaton</option>
				     <option value="smp">Computing the Stabilisation Monoid of a Probabilistic Automaton</option>
				     </select>

<!--				     <select class="matchanger" id="auttype">
				     <option selected="selected" value="det">Deterministic (StarHeight computation)</option>
				     <option value="counter">with Counters (Limitedness test)</option>
				     <option value="proba">Probabilistic (Value 1)</option>
				     </select>
				     -->
</td></tr><tr><td>
				     <b>Letters</b>
				     <select class="matchanger" name="letters" id="lettersnb"></select>
				     <b>states</b>
				     <select class="matchanger" name="states" id="statesnb"></select>
				     <span id="countersnbdiv">
				     <b>counters</b>
				     <select class="matchanger" id="countersnb"></select>
				     </span>
				     </td></tr>
				     <tr><td>
					     	  <input type="button" id="random" value="Random automaton" ></input>

				     <table><tr>
				     <td style="vertical-align:top">
				     <b>Initial</b>
				     <span class="initial"></span>
				     </td><td>&nbsp;</td>
				     <td style="vertical-align:top">
				     <b>Final</b>
				     <span class="final"></span>
				     </td>
				     </td><td>&nbsp;</td>
				     <td>
				     <span id="probamats" ></span>
				     <span id="countermats" ></span>
				     <span id="detmats" ></span>
				     </td>
				     </tr>
				     </table></td></tr>
				     <tr>
				     <td class="density" >Density
				     <select id="density" />
				     <?php
				     for($i=10;$i <=40; $i+=10)
				       echo '<option value="'.$i.'">'.$i.'%</option>';
    echo '<option selected="selected" value="50">50%</option>';
    for($i=60;$i <=90; $i+=10)
      echo '<option value="'.$i.'">'.$i.'%</option>';
    ?>
      </select>
	  </td>
	  </tr>

	  </table>
	  <div id="buttons">
	  <input type="button" class="compute" id="solve" value="Solve" ></input>


	  </div>
	  </form>
	  <div class="log">
	  </div>

	  <h3>Output</h3>
	  <div class="output">
	  </div>
	  <script>
	  var maxstatesnb = 10;
    var maxlettersnb = 4;
    var maxcountersnb = 5;
    var initialstatesnb = 3;
    var initiallettersnb =2;
    var initialcountersnb =2;

	function problem() {
		var ret = $('#problem').val();
		return ret;
	}
	
    function auttype()
    {
    	var pb = problem();
    	if(pb == "vo" || pb == "smp") return "proba";
    	else if(pb == "cl" || pb == "smc") return "counter";
    	else return "det";
    }
    
    function lettersnb()
    {
      return $('#lettersnb').val();
    }
    function statesnb()
    {
      return $('#statesnb').val();
    }
    function countersnb()
    {
      return $('#countersnb').val();
    }

    generate();
    update();
    fill_random();

    setInterval(showProgress, 1000);

    function generate()
    {
      /* Generate statesnb selector */
      $('#statesnb').empty();
      for(i=1; i <= maxstatesnb; i++)
	{
	  var sell = (i == initialstatesnb) ? 'selected="selected"' : '';
	  $('#statesnb').append('<option '+sell+'>'+i+'</option>');
	}

      /* Generate lettersnb selector */
      $('#lettersnb').empty();
      for(i=1; i <= maxlettersnb; i++)
	{
	  var sell = (i == initialstatesnb) ? 'selected="selected"' : '';
	  $('#lettersnb').append('<option '+sell+'>'+i+'</option>');
	}

      /* Generate countersnb selector */
      var cselect = $('#countersnb');
      cselect.empty();
      for(var i = 1 ; i < maxcountersnb; i++)
	{
	  if(i == initialcountersnb)
	    cselect.append('<option selected="selected">'+i+'</option>');
	  else
	    cselect.append('<option>'+i+'</option>');
	} 

      /* Generate initial and fnal  selector */
      $('.initial').empty();
      $('.final').empty();
      for(i=1; i <= maxstatesnb; i++)
	{
	  $('.initial').append('<div  id="i'+i+'">'+i+'<input class="c" type="checkbox"></input></div>');
	  $('.final').append('<div  id="f'+i+'">'+i+'<input class="c" type="checkbox"></input></div>');
	}

      /* Generate matrix editor */
      $('#probamats').empty();
      $('#countermats').empty();
      $('#detmats').empty();
      for(i=1; i <= maxlettersnb; i++)
	{
	  var smat = '<div style="float:left;margin-right:30px" id="Mat'+i+'"><b>Mat '
	    +String.fromCharCode(97 + i - 1)+'</b></div>';
	  $('#probamats').append(smat);
	  $('#countermats').append(smat);
	  $('#detmats').append(smat);
	  var pmat = $('#probamats').find('#Mat'+i);
	  var cmat = $('#countermats').find('#Mat'+i);
	  var dmat = $('#detmats').find('#Mat'+i);
	  pmat.append('<br/>');
	  cmat.append('<br/>');
	  dmat.append('<br/>');
	  for(j=1; j<= maxstatesnb; j++)
	    {
	      var lab = 'l'+i+'_'+j;
	      pmat.append('<div id="'+lab+'">'+j+'</div>');
	      cmat.append('<div id="'+lab+'">'+j+'</div>');
	      dmat.append('<div id="'+lab+'">'+j+'</div>');
	      var pline =pmat.find('#'+lab);
	      var cline =cmat.find('#'+lab);
	      var dline =dmat.find('#'+lab);
		  dline.append('<select selected=""  id="c'+i+'_'+j+'_"/>');
	      for(k=1; k<= maxstatesnb; k++)
		{
		  pline.append('<input selected="" class="c" type="checkbox" id="c'+i+'_'+j+'_'+k+'"/>');
		  cline.append('<select selected=""  id="c'+i+'_'+j+'_'+k+'"/>');
		}
	    }
	}
    }


    function fill_random()
    {
      $('.c').prop('checked',false);
      var ok = false;
      var density = $('#density').val() / 100.0;
      var ccoeff = counters_list();

      /* random choice of final states */
      for(j=1; j<= statesnb; j++)		 
	{
	  var yes = Math.random() < density;
	  $('#f'+j).find("input").prop('checked',yes);
	  ok |= yes;
	}

      /* random choice of one initial state */
      var randinit = 1 + Math.floor( Math.random() * statesnb() );
      $('#i'+randinit).find("input").prop('checked',true);
      //at least one final state or parsing wont work
      if(!ok)
	{
	  randinit = 1 + Math.floor( Math.random() * statesnb() );
	  $('#f'+randinit).find("input").prop('checked',true);
	}

      var res = '';
      var pmat = $('#probamats');
      var cmat = $('#countermats');
      var dmat = $('#detmats');
      for(i=1; i <= maxlettersnb; i++)
	{
	  for(j=1; j<= maxstatesnb; j++)
	    {

		  var dcell = dmat.find('#c'+i+'_'+j+'_');
		  dcell.val( 1 + Math.floor(Math.random()* statesnb()) );

	      for(k=1; k<= maxstatesnb; k++)
		{
		  var pcell = pmat.find('#c'+i+'_'+j+'_'+k);
		  pcell.prop('checked', (Math.random() < density));
		  var ccell = cmat.find('#c'+i+'_'+j+'_'+k);
		  ccell.val( ccoeff[Math.floor(Math.random()*ccoeff.length)] );
		}
	      var cell = pmat.find('#c'+i+'_'+j+'_'+(1 + Math.floor(Math.random()*statesnb)));
	      cell.prop('checked',true);
	    }
	}
    }


    function counters_list()
    {
      var result = [ ["B"," "], ["O","&omega;"] , ["E","&epsilon;"] ];
      var coef = '';
      for(var i = 1; i <= countersnb(); i++)
	{	
	  result.push(['I'+i,'i'+i]);
	  result.push(['R'+i, 'r'+i]);
	}
      return result;
    }


    function update()
    {
      for(i=1; i <= maxstatesnb; i++)
	{
	  if(i <= statesnb())
	    {
	      $('#i'+i).show();
	      $('#f'+i).show();
	    }
	  else
	    {
	      $('#i'+i).hide();
	      $('#f'+i).hide();
	    }		
	}

switch(problem()) {
	case "sh": $('#solve').val("Compute the StarHeight of this Deterministic Automaton"); break;
	case "vo": $('#solve').val("Check whether this Probabilistic Automaton has Value 1"); break;
	case "cl": $('#solve').val("Check whether this Multicounter Automaton is limited"); break;
	case "smp": $('#solve').val("Compute the Stabilisation Monoid of this Probabilistic Automaton"); break;
	case "smc": $('#solve').val("Compute the Stabilisation Monoid of this Multicounter Automaton"); break;
}

      if(auttype() == "proba")
	{	    
	  $('#countersnbdiv').hide();
	  $('#countermats').hide();
	  $('#probamats').show();
	  $('#detmats').hide();
	  $('#probabdiv').show();
	  $('#counterbdiv').hide();
	  $('.density').show();
	}
      else  if(auttype() == "counter")
	{
	  $('#countersnbdiv').show();
	  $('#probamats').hide();
	   $('#detmats').hide();
	 $('#countermats').show();
	  $('#probabdiv').hide();
	  $('#counterbdiv').show();
	  $('.density').hide();
	}       
	else  if(auttype() == "det") 
	{
	  $('#countersnbdiv').hide();
	  $('#probamats').hide();
	   $('#detmats').show();
	  $('#countermats').hide();
	  $('#probabdiv').hide();
	  $('#counterbdiv').hide();
	  $('.density').hide();		
	}

      var cselect = '';
      var counters = counters_list();
      for(var i = 0 ; i < counters.length; i++)
		cselect += '<option value='+counters[i][0]+'>'+counters[i][1]+'</option>';

      var dselect = '';
      for(var i = 0 ; i < statesnb(); i++)
		dselect += '<option value='+(i+1)+'>'+(i+1)+'</option>';
      
      for(i=1; i <= maxlettersnb; i++)
	{
	  var pmat = $('#probamats').find('#Mat'+i);
	  var cmat = $('#countermats').find('#Mat'+i);
	  var dmat = $('#detmats').find('#Mat'+i);

	      if( i > lettersnb()) { pmat.hide(); cmat.hide(); dmat.hide(); }
	      else if(auttype() == "proba") {   pmat.show(); cmat.hide(); dmat.hide();}
	      else if(auttype() == "counter") {  pmat.hide(); cmat.show(); dmat.hide(); }
	      else if(auttype() == "det") {  pmat.hide(); cmat.hide(); dmat.show();}

	      for(j=1; j<= maxstatesnb; j++)
		{
		  var pline = pmat.find('#l'+i+'_'+j);
		  var cline = cmat.find('#l'+i+'_'+j);
		  var dline = dmat.find('#l'+i+'_'+j);
		  if(j > statesnb())  {  pline.hide(); cline.hide(); dline.hide(); }
		  else { pline.show();  cline.show(); dline.show(); }

		      var lab= '#c'+i+'_'+j+'_';
		      var dcell = dmat.find(lab);
		      var old = dcell.val();
				dcell.empty();
		      dcell.append(dselect);
		      dcell.val(old);
		      if(dcell.val() == "")
			    dcell.val("1");
			
		  for(k=1; k<= maxstatesnb; k++)
		    {
		      var lab= '#c'+i+'_'+j+'_'+k;
		      var ccell = cmat.find(lab);
		      var pcell = pmat.find(lab);
		      //		      if(auttype() == "counter")
			
			  /* Updating counters selector */
			  var old = ccell.val();
			  ccell.empty();
			  ccell.append(cselect);
			  ccell.val(old);
			  if(ccell.val() == "")
			    ccell.val(cselect[0][0]);
			
		      if(k <= statesnb()) { ccell.show(); pcell.show(); }
		      else { pcell.hide(); ccell.hide(); }
		    }
		}
       	}	
    }

    function output(data)
    {
      $('.output').empty();
      $('.output').append(data);
    }
    function log(data)
    {
      $('.log').empty();
      $('.log').append(data);
    }

    function showProgress()
    {
      $.ajax({
	method: "POST",
	    url: "",
	    async:false,
	    data: { "action": "progress"}
	})
	.done(function( msg ) {
	    output(msg );
	  });
    }



    $( ".matchanger" ).change(function() {
	update();
      });

    $("#random").click(function() {
	fill_random();
      });

    $(".compute").click(function() {
	var mats = [];
	var initial = '';
	var final = '';
	for(i=1; i <= statesnb(); i++)
	  {
	    var obj = $('#f'+i);
	    if($('#f'+i).find("input").prop('checked')) final += (i-1) + " ";
	    if($('#i'+i).find("input").prop('checked')) initial += (i-1) + " ";
	  }
	for(i=1; i <= lettersnb(); i++)
	  {	 
	    var st = '';
	    for(j=1; j <= statesnb(); j++)
	      {
		for(k=1; k <= statesnb(); k++)
		  {
			
		    if(auttype() == "proba")
		      {
			var $cell = $('#probamats').find('#c'+i+'_'+j+'_'+k);
			st += $cell.prop('checked') ? "1 " : "_ ";
		      }
		    else  if(auttype() == "counter")
		      {
			var $cell = $('#countermats').find('#c'+i+'_'+j+'_'+k);
			st += $cell.val()+ " ";
		      }
		    else  if(auttype() == "det")
		    {
		    }
		  }
		st+="\n";
	      }
	    mats.push(st);
	  }
	var aut = {
	  "type" : (auttype() == "proba") ? "p" : (auttype() == "counter") ? countersnb() : "c",
	  "statesnb" : statesnb(),
	  "lettersnb" : lettersnb(),
	  "initial" : initial,
	  "final" : final,
	  "mats" : mats
	};
	
	$.ajax({
	  method: "POST",
	      url: "",
	      data: { "action": this.id, "automaton" : aut }
	  })
	  .done(function( msg ) {
	      output(msg );
	    });
      });

    </script>
	<br/>
	<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>

<hr/>
<h2 id="Installation">Installation</h2> 
The github directory is <a href="https://github.com/nathanael-fijalkow/stamina">here</a>.
<br/>

    To compile, open a terminal, go to the Stamina directory, type 
<h4>cmake .</h4>
and then 
<h4>make</h4>
<br/>

The command 
<h4>./StaminaTest file.txt -o out.dot</h4>
    reads the automaton from file.txt, and outputs what is computed in out.dot, 
<a href="https://en.wikipedia.org/wiki/DOT_%28graph_description_language%29">a graphic format</a>.
<br/>

    If the automaton is a probabilistic automaton, it runs the Markov Monoid algorithm,
    if it is a classical non-deterministic automaton, it computes its starheight.
<br/>

	    Line by line description of the input file format for automata:
<ul>
   <li> the first line is the size of the automaton (number of states).</li>
   <li> the second line is the type of the automaton: c for classical, p for probabilistic.</li>
											       <li> the third line is the alphabet. Each character is a letter, they should not be separated.</li>
											       <li> the fourth line is the initial states. Each state should be separated by spaces.</li>
											       <li> the fifth line is the final states. Each state should be separated by spaces.</li>
											       <li> the next lines are the transition matrices, one for each letter in the input order.
																			       A transition matrix is given by actions (1 and _) separated by spaces.
																			       Each matrix is preceded by a single character line, the letter (for readability and checking purposes).</li>
</ul>
<br/>
<hr/>
<h2 id="Sage">Sage</h2> 

<p>Stamina can be integrated to <a href="http://www.sagemath.org">Sage</a> as a module.</p>
																			       After compiling Stamina, copy the files stamina.py and libacme.so to sage/stamina-0.1/src/.
<br/>

																			       To create a Sage package:
<h4>$ sage --pkg stamina-0.1</h4>
<br/>

It produces a file stamina-0.1.spkg. It can be installed by
<h4>$ sage -p stamina-0.1.spkg</h4>
<br/>

																			       Now run Sage:
<h4>$ sage</h4>
																			       <h4>sage: import stamina</h4>
																			       <h4>sage: aut = Automaton({0:[(1,'a')],1:[(1,'a')]})</h4>
																			       <h4>sage: aut.state(0).is_initial = True</h4>
																			       <h4>sage: aut.state(1).is_final = True</h4>
																			       <h4>sage: m = stamina.to_monoid(aut)</h4>
																			       <h4>sage: m.has_val1()</h4>
																			       <h4>sage: m.starheight()</h4>
<hr/>

<center>Webmaster: Hugo Gimbert [hugo dot gimbert at cnrs dot fr].</center>

</div>
	</body>
</html>	<?php
	}

?>
