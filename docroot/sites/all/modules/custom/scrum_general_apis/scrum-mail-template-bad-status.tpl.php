<p>
  Hello <?php print $data['scrum_master'] ?>
</p>
<p>
&nbsp;&nbsp;You  scrum master of <span style="color:orange"><?php print $data['project_name'] ?></span>.
</p>
<p>
&nbsp;&nbsp; Sorry to say, your project status is <span style="color:orange"><?php print $data['project_status'] ?></span>.
</p>
<?php if (!empty($data['summary'])): ?>
&nbsp;&nbsp;<h4>Your Project summary is</h4>
<?php print $data['summary'] ?>
<?php endif; ?>
<p>
Thank You<br>
Innoraft solution
</p>
