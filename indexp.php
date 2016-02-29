<html>
<head>
    <title>Temperature, Humidity & Dewpoint</title>
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      google.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
			['datetime', 'temperature °C','humidity %','dewpoint'],
			<?php
			header( "refresh:5;url=indexp.php" );
				$con = mysqli_connect("localhost", "root", "5522", "temp_database");
				
				$query = "SELECT * FROM tempLog";
				$result = mysqli_query($con, $query);
				
				mysqli_close($con);
				
				while ($row = mysqli_fetch_array($result))
				{
					$datetime = $row['datetime'];
					$temperature = $row['temperature'];
					$humidity = $row['humidity'];
					$dewpoint = $row['dewpoint'];
					echo "['$datetime', $temperature,$humidity,$dewpoint],";
				}
			?>
        ]);

        var options = {
          title: 'Temperature, Humidity & Dewpoint',
		  vAxis: { title: "Values" }
        };

        var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
        chart.draw(data, options);
      }
    </script>
</head>
<body>
	<div id="chart_div" style="width: 900px; height: 500px;"></div>
</body>
</html>
