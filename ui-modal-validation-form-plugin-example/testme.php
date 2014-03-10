<script type="text/javascript">
    jQuery(function() {
        var stringMsg = "<strong>Success!</strong> This will disappear in 10 seconds."
        $("#client-script-return-msg-rtn").html(stringMsg);
    });
</script>

<div>
<label>Name: <?php echo $_POST["fullname"];  ?></label>
<label>Email: <?php echo $_POST["email"]; ?></label>
</div>