<!DOCTYPE html>

<html>

    <head>

        <meta charset="utf-8" />

        <title>TP eApplication</title>
	<meta hhtp-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
	

    </head>


    <body>
      <form id="recherche" name="recherche" method="get" action="http://www.jeuxdemots.org/rezo-dump.php">
	<lablel>Terme à chercher</label>:<input type="text" id="recherche" name="recherche">
<input id="searchsubmit" name="searchsubmit" value="Chercher" type="submit">
<br>
<lablel>Option: numéro de relation</label>:<input type="text" id="relation" name="relation">
	</form>
<?php
   ob_start();
   echo 'test';
   $tmp = ob_get_contents();
   file_put_contents('cache/index.html', $tmp);
   ob_end_clean();
   ?>
    <?php
   readfile('cache/index.html);
  ?>

    </body>

</html>
