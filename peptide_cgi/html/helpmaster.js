// --- helpmaster.js
// --- by Josef Becker, Mediendidaktische Beratung
// --- http://www.helpmaster.com

// --- popupon and popupoff base on a script written by Andrew Castles bound to use with Dreamweaver 1.2
// --- http://www.arrakis.es/~andrewc/downloads/archives/tooltipdemo.htm

function popupon(text, eventObj) 
{ 
  ieLayer = 'document.all[\'popup\']';
  nnLayer = 'document.layers[\'popup\']';

  //-- Start Editable Area --
  borderColor = '#000000';
  bgColor = '#ffffcc';
  border = 1;
  padding = 3;
  xOffset = 3;
  yOffset = 3;
  Font = 'face="Verdana, Arial, Helvetica, sans-serif" size=2';
  //-- End Editable Area --

  if (!(document.all || document.layers)) return;
  if (null != document.all) document.popup = eval(ieLayer); else document.popup = eval(nnLayer);

  var table = "";
  var bigTable = ""; // Workaround for Netscape

  if (null != document.all) 
    { // If IE4+
    table += "<table bgcolor= "+ bgColor +" border= "+ border +" cellpadding= "+ padding +" cellspacing=0>";
    table += "<tr><td>";
    table += "<table cellspacing=0 cellpadding="+ padding +">";
    table += "<tr><td bgcolor= "+ bgColor +"><font "+ Font +">" + text + "</font></td></tr>";
    table += "</table></td></tr></table>"
    } 
  else 
    { // If NN4+
    table += "<table cellpadding="+ padding +" border="+ border +" cellspacing=0 bordercolor="+ borderColor +">";
    bigTable += "<table width="+(document.width - xOffset - eventObj.layerX - 30)+"cellpadding="+ padding +" border="+ border +" cellspacing=0 bordercolor="+ borderColor +">";
    table += "<tr><td bgcolor="+ bgColor +"><font "+ Font +">" + text + "</font></td></tr></table>";
    bigTable += "<tr><td bgcolor="+ bgColor +"><font "+ Font +">" + text + "</font></td></tr></table>";
    }

  if (null != document.all) 
    { // If IE4+
    document.popup.innerHTML = table;
    document.popup.style.left = eventObj.x + xOffset;
    document.popup.style.top  = eventObj.y + yOffset;
    document.popup.style.visibility = "visible";
    } 
  else 
    { // If NN4+
    document.popup.document.open();
    document.popup.document.write(table);
    document.popup.document.close();
    if ((document.popup.document.width + xOffset + eventObj.layerX) > document.width)
    { // If the layer runs off the right hand side
      document.popup.document.open();
      document.popup.document.write(bigTable);
      document.popup.document.close();
    }
    document.popup.left = eventObj.layerX + xOffset;
    document.popup.top  = eventObj.layerY + yOffset;
    document.popup.visibility = "visible";
  }
}


function popupoff() 
{ 
  if (!(document.all || document.layers)) return;
  if (null == document.popup) {
  } else if (null != document.all)
    document.popup.style.visibility = "hidden";
  else
    document.popup.visibility = "hidden";
    document.popup = null;
}