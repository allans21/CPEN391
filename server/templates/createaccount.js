

var usernam = document.getElementById("newusername");
var userpass = document.getElementById("newpassword");
var useremail = document.getElementById("newemail");
var userphone = document.getElementById("newphone");
var userID = document.getElementById("newID");

var createAccount = document.getElementById("createAcc");

createAccount.addEventListener("click", createAcc);

function createAcc(){
	var newusername = usernam.value;
	var newpassword = userpass.value;
	var newuseremail = useremail.value;
	var newuserphone = userphone.value;
	var newID = userID.value;

	form.submit();
	alert("username is: " + newusername + "\npassword is: " + newpassword + "\nemail is: " + newuseremail + 
		  "\nphone number is: " + newuserphone + "\n ID is: " + newID);
	window.open("CustomersAccount.html");
}
