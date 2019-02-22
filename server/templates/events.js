var signin = document.getElementById("signin");
var passwordEl = document.getElementById("userpass");
var usernameEl = document.getElementById("usernam");
var form = document.getElementById("signin-form");

signin.addEventListener("click", signingIn);
function signingIn() {
	var password = passwordEl.value;
	var username = usernameEl.value;
	form.submit();
	// alert("username is: " + username + "\npassword is: " + password);
	// window.open("CustomersAccount.html");
}
