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
      $('#detmats').append("<div><b>Transitions</b></div>");
      
      for(i=1; i <= maxlettersnb; i++)
	{
				var l = String.fromCharCode(97 + i - 1);
	  var smat = '<div style="float:left;margin-right:30px" id="Mat'+i+'"><b>Mat '+l+'</b></div>';
	  $('#probamats').append(smat);
	  $('#countermats').append(smat);
      smat = '<div style="float:left;margin-right:30px" id="Mat'+i+'"></div>';
	  $('#detmats').append(smat);
	  var pmat = $('#probamats').find('#Mat'+i);
	  var cmat = $('#countermats').find('#Mat'+i);
	  var dmat = $('#detmats').find('#Mat'+i);
	  pmat.append('<br/>');
	  cmat.append('<br/>');
	  for(j=1; j<= maxstatesnb; j++)
	    {
	      var lab = 'l'+i+'_'+j;
	     dmat.append('<div style="float:top;" id="'+lab+'"></div>');
	      pmat.append('<div id="'+lab+'">'+j+'</div>');
	      cmat.append('<div id="'+lab+'">'+j+'</div>');

	      var dline =dmat.find('#'+lab);
		  dline.append('<b> '+j + ',' +l+' &#8614; </b> <select selected=""  id="c'+i+'_'+j+'_"/>');

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
			  if(j > statesnb()) {
		      lab= '#l'+i+'_'+j;
		      	dline = dmat.find(lab);
			     dline.hide();
			    }
			
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
			var $cell = $('#detmats').find('#c'+i+'_'+j+'_');
			st += ($cell.val() == k ) ? "1 " : "_ ";
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
	      data: { "action": this.id, "automaton" : aut, "problem" : problem() }
	  })
	  .done(function( msg ) {
	      output(msg );
	    });
      });
