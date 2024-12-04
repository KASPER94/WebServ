<?php
session_start();
if (isset($_GET['deleteSession']) && $_GET['deleteSession'] == 'true') {
	session_unset();
	session_destroy();
} else {
	if (isset($_GET['fName']) && $_GET['fName'] != '') {
		$_SESSION['fName'] = $_GET['fName'];
	}
}
?>
<!DOCTYPE html>
<html>

<head>
	<title>Session</title>
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css">
	<link rel="preconnect" href="https://fonts.googleapis.com">
	<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
	<link href="https://fonts.googleapis.com/css2?family=Roboto+Mono:ital,wght@0,100..700;1,100..700&display=swap" rel="stylesheet">
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
</head>

<body <?php if (isset($_SESSION['bg'])) { ?>style="background-color:#<?php echo $_SESSION['bg'] ?> !important;" <?php } ?>>
	<div class="container p-3">
		<?php
		?>
		<div class="rounded-3 p-5" style="background-color:#fcfcfc; border:solid 1px #cdcdcd">
			<center>
				<h2>Session</h2>
			</center>
			<div class="d-flex flex-row justify-content-around p-3 rounded-3" style="background-color: #ededed;">
				<span>Your background color is : <b><?php if (isset($_SESSION['bg'])) {
												echo "#" . $_SESSION['bg'];
											} else {
												echo "undefined";
											} ?></b></span>
			</div>
			<br />
			<div class="d-flex flex-row justify-content-around align-items-center p-2 rounded-3" style="background-color: #ededed;">
				<input id="firstName" name="firstName" type="type" class="form-control"
						style="margin-right: 10px;"
						placeholder="First Name : <?php if (isset($_SESSION['fName'])) {
							echo $_SESSION['fName'];
						} ?>" />
				<button class="btn btn-primary" id="saveName">Save</button>
			</div>
			<br />
			<div class="d-flex flex-row justify-content-around p-3 rounded-3" style="background-color: #ededed;">
				<span>Your last name is : <b><?php if (isset($_SESSION['lName'])) {
										echo $_SESSION['lName'];
									} else {
										echo "undefined";
									} ?></b></span>
			</div>
			<br />
			<div class="d-flex flex-row justify-content-around p-2 rounded-3" style="background-color: #ededed;">
				<a class="btn btn-primary" href="/." style="width: 150px">Home</a>
				<a class="btn btn-secondary" href="/firstName" style="width: 150px">First Name</a>
				<a class="btn btn-success" href="/lastName" style="width: 150px">Last Name</a>
			</div>
			<br />
			<div class="d-flex flex-row justify-content-around p-2 rounded-3" style="background-color: #ededed;">
				<a class="btn btn-danger" href=".?deleteSession=true">Delete session</a>
			</div>
		</div>
	</div>
</body>
<script type="text/javascript">
	$(document).ready(function() {
		$("#saveName").on("click", function() {
			var fName = $("#firstName").val();
			location.href = "/firstName/?fName=" + fName;
		});
	});
</script>

</html>
