<p>
  Hello Admin
    Following scrums have bad/worse status in their last scrum.
</p>

<?php foreach($data as $value): ?>

  <p>
    <h4>Project Name</h4> : <?php print $value['project_name'] ?>
  </p>
  <p>
    <h4>Scrum Master</h4> : <?php print $value['scrum_master'] ?>
  </p>
  <p>
    <h4>Project Status<h4/> : <?php print $value['project_status'] ?></span>.
  </p>
  <p>
    <h4>Scrum Date<h4/> : <?php print $value['date'] ?></span>.
  </p>
  <?php if (!empty($value['summary'])): ?>
  <h4>Project summary</h4> :
  <?php print $value['summary'] ?>
  <?php endif; ?>


<?php endforeach; ?>

<p>
Thank You<br>
Innoraft solution
</p>
