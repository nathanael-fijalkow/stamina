<?php
session_start();
$iid = session_id();
		
$dir = $iid;
$input="automaton.".$dir.".acme";
$output="computation.".$dir.".log";

if(isset($_POST['action']))
{
	$action = $_POST['action'];
	switch($action)
	{
		case 'compute':
		     $aut=$_POST['automaton'];

		     //starts a new computation
		     //we create the computation dir
//		     if(!is_dir($dir))
//		         mkdir($dir,"0777",true);
		     //we create the input file


		     $f = fopen($input,"w+");
		     $o = fopen($output,"w+");
		     fwrite($o,"Parsing data...\n");
		     if($f === FALSE)
		     {
		     	     $msg = "Failed to create input automaton file";
		     		fwrite($o,$msg);
		     		echo $msg;
				break;
			}
			else if($aut["initial"] == '')
			{
				$msg = "The automaton has no initial state, the value is 0";
		     		fwrite($o,$msg);
				break;
		        }
		      	  else if($aut["final"] == '')
			  {
				$msg = "The automaton has no final state, the value is 0";
		     		fwrite($o,$msg);
				break;
			}
			else
			{

			//we send data to the file
			$snb = $aut["statesnb"];
			$lnb = $aut["lettersnb"];
			fwrite($f,$snb."\np\n");
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
			fwrite($o,'The <a href="'.$input.'"> automaton file</a>'."\n"); 
			fwrite($o,'The <a href="'.$output.'"> complete log file</a>'."\n"); 
			fwrite($o,"******************************************\n");
			fwrite($o,"Computation started\n");
			fwrite($o,"******************************************\n");
			fclose($o);
			echo "Computation started";	
			exec("/usr/local/bin/WebDemo ".$input." >> ".$output. " 2>/dev/null  &");
			}
		      break;
		case 'progress':
		     //retourne le contenu du fichier de sortie
		     $f =fopen($output,"r");
		     if($f ===false)
		     {
			echo "No output yet";
			break;
		     } 
		     $out = "";
		     $total = 0;
		     $max = 100000;
		     while(true)
		     {
			$chunk  = fread($f,1024);
			if($chunk == "")	
				break;				
			echo str_replace("\n","<br/>",$chunk);		
			$total += strlen($chunk);
			if($total > $max)
			{
				echo "Computation too long, output truncated.<br/>";
				echo "Please download the log file for a complete log.<br/>";
				break;
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
<head>
<script src="jquery-2.2.0.min.js"></script>
</head>
<body>
<h1>Demo page of Acme++</h1>


<p>Welcome on the demo page of Acme++,
a tool manipulating stabilization monoids in order to solve three algorithmic problems.
</p>
<ul>
<li> Computing the star-height of a regular language, i.e. the minimal number of nested Kleene stars needed for expressing the language with a regular expression.
</li>
<li> Deciding boundedness and equivalence for regular cost functions.
</li>
<li>
Deciding whether a leaktight probabilistic automaton has value 1, i.e. whether the automaton accepts some words with probability arbitrarily close to 1.
</li>
</ul>
These three problems reduce to the computation of the stabilization monoid associated with the automaton, which is a challenge since the monoid is exponentially larger than the automaton. The compact data structures used in Acme++, together with optimizations and heuristics, allow this program to handle automata with several hundreds of states.
</p>
<p>
The project source code is available <a href="https://github.com/nathanael-fijalkow/acmeplusplus">on Github</a>.
</p>
<p>
The following form allows you to specify a small probabilistic automaton
and compute a presentation of the associated Markov monoid.
</p>
<form method="post">
<table>
<tr>
<td>
<b>Number of states</b>
<select name="states" class="statesnb">
</select>
<b> letters</b>
<select name="letters" class="lettersnb">
</select>
</td></tr>
<tr><td>
<b>Initial states</b>
<div class="initial">
</div>
</tr></td>
<tr><td>
<b>Final states</b>
<div class="final">
</div>
</tr></td>
<tr>
<td class="mats">
</td>
</tr>
<tr>
<td>Density
<select id="density">
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
</form>
<h2>Output</h2>
<div class="output">
</div>
<script>
var statesnb = 3;
var maxstatesnb = 10;
var lettersnb = 2;
var maxlettersnb = 4;

function generate_statesnb_select()
{
	$('.statesnb').empty();
	for(i=1; i <= maxstatesnb; i++)
	{
		if(i == statesnb)
	     	     $('.statesnb').append('<option selected="selected">'+i+'</option>');
		else
			$('.statesnb').append('<option>'+i+'</option>');
	}
	$('.lettersnb').empty();
	for(i=1; i <= maxlettersnb; i++)
	{
		if(i == lettersnb)
	     	     $('.lettersnb').append('<option selected="selected">'+i+'</option>');
		else
			$('.lettersnb').append('<option>'+i+'</option>');
	}
}
function generate_initial()
{
	$('.initial').empty();
	$('.final').empty();
	for(i=1; i <= maxstatesnb; i++)
	{
		$('.initial').append('<div style="float: left" id="i'+i+'"><input class="c" type="checkbox">'+i+'</input></div>');
		$('.final').append('<div style="float: left" id="f'+i+'"><input class="c" type="checkbox">'+i+'</input></div>');
	}
	update_initial();
}
function update_initial()
{
	for(i=1; i <= maxstatesnb; i++)
	{
		if(i <= statesnb)
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
}
function fill_random()
{
	$('.c').prop('checked',false);
	var ok = false;
	var density = $('#density').val() / 100.0;
	for(j=1; j<= statesnb; j++)		 
	{
		var yes = Math.random() < density;
		$('#f'+j).find("input").prop('checked',yes);
		ok |= yes;
	}
	var randinit = 1 + Math.floor(Math.random()*statesnb);
	$('#i'+randinit).find("input").prop('checked',true);
	//at least one final state or parsing wont work
	if(!ok)
	{
		randinit = 1 + Math.floor(Math.random()*statesnb);
		$('#f'+randinit).find("input").prop('checked',true);
	}

	var res = '';
	for(i=1; i <= lettersnb; i++)
	{
		var mat = $('#Mat'+i);
		for(j=1; j<= statesnb; j++)
		{
			for(k=1; k<= statesnb; k++)
			{
				var cell = $('#c'+i+'_'+j+'_'+k);
				cell.prop('checked', (Math.random() < density));
			}
			var cell = $('#c'+i+'_'+j+'_'+(1 + Math.floor(Math.random()*statesnb)));
			cell.prop('checked',true);
		}
	}
}
function update_mats()
{
	for(i=1; i <= maxlettersnb; i++)
	{
		var mat = $('#Mat'+i);
		if(i <= lettersnb)
		{
			mat.show();
			for(j=1; j<= maxstatesnb; j++)
			{
				var line = $('#l'+i+'_'+j);
				if(j > statesnb)
				{
					line.hide();
				}
				else
				{		
					line.show();
				 	for(k=1; k<= maxstatesnb; k++)
				 	{
						var cell = $('#c'+i+'_'+j+'_'+k);
				 		if(k <= statesnb)
						     cell.show();
	  		  			else
							cell.hide();
					}
				}
			}
		}
		else
			mat.hide();
	}
}

function generate_mats()
{
	$('.mats').empty();
	for(i=1; i <= maxlettersnb; i++)
	{
		$('.mats').append('<div style="float:left;margin-right:30px" id="Mat'+i+'"><b>Mat '
					+String.fromCharCode(97 + i - 1)+'</b></div>');
		var mat = $('#Mat'+i);
		mat.append('<br/>');
		for(j=1; j<= maxstatesnb; j++)
		{
			var lab = 'l'+i+'_'+j;
			mat.append('<div id="'+lab+'">'+j+'</div>');
			var line =$('#l'+i+'_'+j);
			for(k=1; k<= maxstatesnb; k++)
			{
				line.append('<input class="matel" checked="checked" type="checkbox" id="c'+i+'_'+j+'_'+k+'"></input>');
			}
		}
	}
}
function output(data)
{
	$('.output').empty();
	$('.output').append(data);
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

$('form').append('<input type="button" class="random" value="Random" />');
$('form').append('<input type="button" class="compute" value="Check value 1" />');
generate_statesnb_select();
generate_initial();
generate_mats();
update_mats();
fill_random();

setInterval(showProgress, 500);

	
$( ".statesnb" ).change(function() {
   statesnb=this.value;
   generate_statesnb_select();
   update_initial();
   update_mats();
});

$( ".lettersnb" ).change(function() {
   lettersnb=this.value;
   generate_statesnb_select();
   update_mats();
});

$(".random").click(function() {
	fill_random();
});

$(".compute").click(function() {
    var mats = [];
    var initial = '';
    var final = '';
    	for(i=1; i <= statesnb; i++)
	{
		var obj = $('#f'+i);
		if($('#f'+i).find("input").prop('checked')) final += (i-1) + " ";
		if($('#i'+i).find("input").prop('checked')) initial += (i-1) + " ";
	}
	for(i=1; i <= lettersnb; i++)
	{	 
		var st = '';
		for(j=1; j <= statesnb; j++)
		{
			for(k=1; k <= statesnb; k++)
			{
			 var $cell = $('#c'+i+'_'+j+'_'+k);
			 st += $cell.prop('checked') ? "1 " : "_ ";
			}
			st+="\n";
		}
		mats.push(st);
	}
	var aut = {
	"statesnb" : statesnb,
	"lettersnb" : lettersnb,
	"initial" : initial,
	"final" : final,
	"mats" : mats
	};
	
	$.ajax({
	  method: "POST",
	    url: "",
	      data: { "action": "compute", "automaton" : aut }
	      })
	        .done(function( msg ) {
		    output(msg );
		      });
});

$( "input" ).change(function() {
  var $input = $( this );
  output(
	$input.attr("id") + $input.prop("checked")
	);
}).change();

</script>
<br/>
<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>

</body>
<?php
}
?>
