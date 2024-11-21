<?php
session_start();
if (isset($_GET['deleteSession']) && $_GET['deleteSession'] == 'true') {
	session_unset();
	session_destroy();
} else {
	if (isset($_GET['bg']) && $_GET['bg'] != '') {
		$_SESSION['bg'] = $_GET['bg'];
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
</head>

<body <?php if (isset($_SESSION['bg'])) { ?>style="background-color:#<?php echo $_SESSION['bg'] ?> !important;" <?php } ?>>
	<div class="container p-3">
		<?php
		?>
		<div class="rounded-3 p-5" style="background-color:#fcfcfc; border:solid 1px #cdcdcd">
			<center>
				<h2>Session</h2>
			</center>
			<div class="d-flex flex-row justify-content-around p-2 rounded-3" style="background-color: #ededed;">
				<a class="btn btn-primary" href=".?bg=0b5ed7">Blue</a>
				<a class="btn btn-secondary" href=".?bg=5c636a">Gray</a>
				<a class="btn btn-success" href=".?bg=157347">Green</a>
				<a class="btn btn-danger" href=".?bg=bb2d3b">Red</a>
				<a class="btn btn-warning" href=".?bg=ffca2c">Yellow</a>
				<a class="btn btn-info" href=".?bg=31d2f2">Cyan</a>
				<a class="btn btn-light" href=".?bg=f9fafb">White</a>
				<a class="btn btn-dark" href=".?bg=1c1f2a">Black</a>
			</div>
			<br />
			<div class="d-flex flex-row justify-content-around p-3 rounded-3" style="background-color: #ededed;">
				<span>Your first name is : <b><?php if (isset($_SESSION['fName'])) {
											echo $_SESSION['fName'];
										} else {
											echo "undefined";
										} ?></b></span>
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

</html>
