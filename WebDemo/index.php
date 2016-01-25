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
			echo str_replace(array("\n","E ","O ","B "),array("<br/>","&epsilon;","&omega;","&infin;"),$chunk);		
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
<b>Type of automaton</b>
<select class="auttype">
<option value="proba">Probabilistic</option>
<option value="counter">with Counters</option>
</select>
</tr>
<tr>
<td>
<div id="countersnbdiv">
<b>Number of counters</b>
<select id="countersnb"/>
</div>
</td>
</tr>
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
<td>
<div id="probamats">
</div>
<div id="countermats">
</div>
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
<div id="buttons">
<input type="button" class="random" value="Random" />
<div style="float: left" id="probabdiv">
<input type="button" class="compute" value="Check value 1" />
</div>
<div style="float: left" id="counterbdiv">
<input type="button" class="compute" value="Check boundedness" />
</div>


</div>
</form>
<h2>Output</h2>
<div class="output">
</div>
<script>
var statesnb = 3;
var maxstatesnb = 10;
var lettersnb = 2;
var maxlettersnb = 4;
var auttype = "proba";
var maxcountersnb = 5;
var countersnb = 2;

generate_statesnb_select();
generate_initial();
set_type();
generate_mats();

//setting initial value to 0 enforces generation of the form
countersnb= 0;
update_mats(2);

fill_random();

setInterval(showProgress, 500);

function set_type()
{
    auttype = $('.auttype').val();

    if(auttype == "proba")
    {	    
    	$('#countersnbdiv').hide();

	$('#countermats').hide();
	$('#probamats').show();
    	$('#probabdiv').show();
	$('#counterbdiv').hide();
	$('#density').show();
	}
    else
    {
  	$('#countersnbdiv').show();
	$('#probamats').hide();
	$('#countermats').show();
	$('#probabdiv').hide();
	$('#counterbdiv').show();
	$('#density').hide();
    }
}

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
	var cselect = $('#countersnb');
	cselect.empty();
	for(var i = 1 ; i < maxcountersnb; i++)
	{
		if(i == countersnb)
			cselect.append('<option selected="selected">'+i+'</option>');
		else
			cselect.append('<option>'+i+'</option>');
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
	var ccoeff = counters_list();

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
	var pmat = $('#probamats');
	var cmat = $('#countermats');
	for(i=1; i <= lettersnb; i++)
	{
		for(j=1; j<= maxstatesnb; j++)
		{
			for(k=1; k<= maxstatesnb; k++)
			{
				var pcell = pmat.find('#c'+i+'_'+j+'_'+k);
				pcell.prop('checked', (Math.random() < density));
				var ccell = cmat.find('#c'+i+'_'+j+'_'+k);
				ccell.val(ccoeff[Math.floor(Math.random()*ccoeff.length)]);
			}
			var cell = pmat.find('#c'+i+'_'+j+'_'+(1 + Math.floor(Math.random()*statesnb)));
			cell.prop('checked',true);
		}
	}
}

function setCharAt(str,index,chr) {
    if(index > str.length-1) return str;
    return str.substr(0,index) + chr + str.substr(index+1);
}

function next_coef(coef)
{
	if(coef == "")
		return "";
	for(var i = coef.length -1; i >= 0; i--)
	{
		switch(coef[i])
		{
		case 'O': 
		     return setCharAt(coef,i,'E');
		case 'E': 
		     return setCharAt(coef,i,'I');
		case 'I': 
		     return setCharAt(coef,i,'R');
		case 'R':
			if(i == 0) 
			     return "";
			coef[i]='O';
		}
	}
	return "";
}

function counters_list()
{
	var result = [ ["B","&infin;"], ["O","&omega;"] , ["E","&epsilon;"] ];
	var coef = '';
	for(var i = 1; i <= countersnb; i++)
	{	
		result.push(['I'+i,'i'+i]);
		result.push(['R'+i, 'r'+i]);
	}
	return result;
}

function generate_counters_select()
{
	var result = '';
	var counters = counters_list();
	for(var i = 0 ; i < counters.length; i++)
		result += '<option value='+counters[i][0]+'>'+counters[i][1]+'</option>';
	return result;
}
function update_mats(ncountersnb)
{
	var counterschanged = (ncountersnb != countersnb);
	countersnb = ncountersnb;

	var cselect = counterschanged ? generate_counters_select() : '' ;

	for(i=1; i <= maxlettersnb; i++)
	{
		var pmat = $('#probamats').find('#Mat'+i);
		var cmat = $('#countermats').find('#Mat'+i);
		if(i <= lettersnb)
		{
			if(auttype == "proba")
			{ 
			  pmat.show();
			  cmat.hide();
			}
			else
			{
			  pmat.hide();
			  cmat.show();
			}

			for(j=1; j<= maxstatesnb; j++)
			{
				var pline = pmat.find('#l'+i+'_'+j);
				var cline = cmat.find('#l'+i+'_'+j);
				if(j > statesnb)
				{
					pline.hide();
					cline.hide();
				}
				else
				{		
					pline.show();
					cline.show();
				 	for(k=1; k<= maxstatesnb; k++)
				 	{
						var lab= '#c'+i+'_'+j+'_'+k;
						var ccell = cmat.find(lab);
						var pcell = pmat.find(lab);
						if(counterschanged)
						{
							var old = ccell.val();
							ccell.empty();
							ccell.append(cselect);
							ccell.val(old);
							if(ccell.val() == "")
							{
								cmat.find(lab +' :nth-child(0)').prop('selected', true);
							}
						}
				 		if(k <= statesnb)
						{
						     ccell.show();
    						     pcell.show();
						}
	  		  			else
						{
						     pcell.hide();
    						     ccell.hide();
						}
					}
				}
			}
		}
		else
		{
			pmat.hide();
			cmat.hide();
		}
	}	
}

function generate_mats()
{
	$('#probamats').empty();
	$('#countermats').empty();
	for(i=1; i <= maxlettersnb; i++)
	{
		var smat = '<div style="float:left;margin-right:30px" id="Mat'+i+'"><b>Mat '
					+String.fromCharCode(97 + i - 1)+'</b></div>';
		$('#probamats').append(smat);
		$('#countermats').append(smat);
		var pmat = $('#probamats').find('#Mat'+i);
		var cmat = $('#countermats').find('#Mat'+i);
		pmat.append('<br/>');
		cmat.append('<br/>');
		for(j=1; j<= maxstatesnb; j++)
		{
			var lab = 'l'+i+'_'+j;
			pmat.append('<div id="'+lab+'">'+j+'</div>');
			cmat.append('<div id="'+lab+'">'+j+'</div>');
			var pline =pmat.find('#'+lab);
			var cline =cmat.find('#'+lab);
			for(k=1; k<= maxstatesnb; k++)
			{
				pline.append('<input selected="" class="c" type="checkbox" id="c'+i+'_'+j+'_'+k+'"/>');
				cline.append('<select selected=""  id="c'+i+'_'+j+'_'+k+'"/>');
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



$( ".statesnb" ).change(function() {
   statesnb=this.value;
   update_initial();
   update_mats(countersnb);
});

$( ".lettersnb" ).change(function() {
   lettersnb=this.value;
   update_mats(countersnb);
});

$( ".countersnb" ).change(function() {
   update_mats(countersnb);
});

$( ".auttype" ).change(function() {
   set_type();
   update_mats(countersnb);
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
			
			if(auttype == "proba")
			{
			 var $cell = $('#probamats').find('#c'+i+'_'+j+'_'+k);
			 st += $cell.prop('checked') ? "1 " : "_ ";
			 }
			 else
			 {
			 var $cell = $('#countermats').find('#c'+i+'_'+j+'_'+k);
			 st += $cell.val()+ " ";
			 }
			}
			st+="\n";
		}
		mats.push(st);
	}
	var aut = {
	"type" : (auttype == "proba") ? "p" : countersnb,
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

</script>
<br/>
<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>

</body>
<?php
}
?>
