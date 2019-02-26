var signin = document.getElementById("signin");
var signup = document.getElementById("createAcc");

var form2 = document.getElementById("signup-form");
var form = document.getElementById("signin-form");

signup.addEventListener("click", signingUp);
signin.addEventListener("click", signingIn);

function signingIn() {
	form.submit();
	// alert("username is: " + username + "\npassword is: " + password);
	// window.open("CustomersAccount.html");
}

function signingUp(){
	alert("here");
	document.cookie = "access_token={{ access_token }}";
	form2.submit();
}