var signin = document.getElementById("signin");
var passwordEl = document.getElementById("userpass");
var usernameEl = document.getElementById("usernam");

var signup = document.getElementById("createAcc");
var addr = document.getElementById("newAddress");
var email = document.getElementById("newemail");
var pass = document.getElementById("newpassword");
var name = document.getElementById("newName");
var liID = document.getElementById("newID");
var Card = document.getElementById("newCard");
var phone = document.getElementById("newphone");

var form2 = document.getElementById("signup-form");
var form = document.getElementById("signin-form");

signup.addEventListener("click", signingUp);
signin.addEventListener("click", signingIn);

function signingIn() {
	var password = passwordEl.value;
	var username = usernameEl.value;
	form.submit();
	// alert("username is: " + username + "\npassword is: " + password);
	// window.open("CustomersAccount.html");
}

function signingUp(){
	alert("changing cookie {{ access_token }}");
	document.cookie = "access_token={{ access_token }}";
	form2.submit();
}