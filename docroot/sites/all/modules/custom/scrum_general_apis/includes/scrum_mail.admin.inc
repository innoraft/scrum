<?php
/**
 * @file
 * Module implementing scrum related functionality.
 */

function scrum_mail_variable($form, &$form_state) {

  $form['scrum_mail_time'] = array(
    '#type' => 'textfield',
    '#title' => t('Mail time'),
    '#default_value' => variable_get('scrum_mail_time', 18),
    '#description' => t("Time should be 24H Format"),
    '#required' => TRUE,
    '#element_validate' => array('scrum_email_time_element_validate'),
  );

  $form['scrum_mail_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Scrum mail subject'),
    '#default_value' => variable_get('scrum_mail_subject', 'Create you scrum'),
    '#description' => t("Scrum mail subject"),
    '#required' => TRUE,
  );

  $form['scrum_mail_cc'] = array(
    '#title' => t('Mail Cc'),
    '#type' => 'textarea',
    '#default_value' => variable_get('scrum_mail_cc', 'mukesh.agarwal@innoraft.com,basant.sharma@innoraft.com'),
    '#description' => t('You can provides multiple value by comma-separated.'),
    '#required' => TRUE,
    '#element_validate' => array('scrum_email_element_validate'),
  );

  $form['scrum_mail_body'] = array(
    '#title' => t('Mail Body'),
    '#type' => 'textarea',
    '#default_value' => variable_get('scrum_mail_body', "Please create today's scrum"),
    '#description' => t('Please provide mail body.'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}

/**
 * Validate scrum mail field.
 */
function scrum_email_element_validate($element, &$form_state, $form) {

  $mail_cc = $element['#value'];
  $mail_cc = trim($mail_cc);
  if ($mail_cc !== '') {
    $mails = explode(',', $mail_cc);
    foreach ($mails as $key => $mail) {
      if (!valid_email_address($mail)) {
        form_error($element, t('Mail id are not correct, only comma seperated values are allowed'));
      }
    }
  }
}

/**
 * Validate scrum mail time field.
 */
function scrum_email_time_element_validate($element, &$form_state, $form) {

  $mail_time = $element['#value'];
  if (!(is_numeric($mail_time) && $mail_time > 0 && $mail_time < 24)) {
    form_error($element, t('Time should be 0 to 24'));
  }
}

/**
 * Send mail who not created scrum for today.
 * @param array $tids
 *   Having projects tids.
 */
function scrum_send_mail_for_today_scrum($tids) {

  $start = date('U', mktime(0, 0, 0, date('n'), date('j'), date('Y')));
  $end = date('U', mktime(23, 59, 59, date('n'), date('j'), date('Y')));
  $exclude_tids = array();
  foreach ($tids as $tid) {
    $query = new EntityFieldQuery();
    $result = $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'ct_scrum')
    ->propertyCondition('status', NODE_PUBLISHED)
    ->fieldCondition('field_ct_scrum', 'tid', $tid)
    ->range(0, 1)
    ->propertyCondition('created', array($start, $end), "between")
    ->execute();

    if (empty($result)) {
      $exclude_tids[] = $tid;
    }
  }

  if (!empty($exclude_tids)) {
    $terms = taxonomy_term_load_multiple($exclude_tids);
    $mail_ids = array();
    $scrum_mail_detail = array();
    foreach ($terms as $term) {
      $user_id = field_get_items('taxonomy_term', $term, 'field_ct_scrum_master');
      $scrum_mail_detail[$term->tid]['project_name'] = $term->name;

      if (isset($user_id[0]['target_id'])) {
        $uid = $user_id[0]['target_id'];
        $user = user_load($uid);
        $mail_ids[] = isset($user->mail) ? $user->mail : NULL;
        $scrum_mail_detail[$term->tid]['email'] = isset($user->mail) ? $user->mail : NULL;
        $scrum_mail_detail[$term->tid]['scrum_master'] = isset($user->name) ? $user->name : NULL;
      }
    }

    foreach ($scrum_mail_detail as $key => $value) {
      $mail_to = isset($value['email']) ? $value['email'] : NULL;
      if (!is_null($mail_to)) {
        $from = variable_get('site_mail', '');
        $to = $mail_to;
        $ln = language_list();
        $from = $from;
        $language = $ln['en'];
        $params['subject'] = $value['project_name'] . ' -  Scrum Not Done';

        $scrum_template = theme('scrum_mail_template', array('data' => $value));
        $params['body'][0] = $scrum_template;
        $mail_cc = variable_get('scrum_mail_cc', 'mukesh.agarwal@innoraft.com,basant.sharma@innoraft.com');
        $params['headers'] = array(
          'Cc' => $mail_cc,
          'MIME-Version' => '1.0',
          'Content-Type' => 'text/html; charset=UTF-8; format=flowed',
        );

        drupal_mail('scrum_general_apis', 'scrum_mail_key', $to, $language, $params, $from, TRUE);
      }
    }
  }
}


function scrum_send_mail_for_bad_scrum($tids) {
  // Current date 00:00:00 AM unix timestamp value.
  $start = strtotime('today midnight');

  // Current date 23:59:59 PM unix timestamp value.
  $end = $start + 86399;

  // Previous date 00:00:00 AM unix timestamp value.
  $start = $start - 86400;

  // Previous date 23:59:59 PM unix timestamp value.
  $end = $start + 86399;

  // Get Bad and worst term's tid.
  $status_vocab = taxonomy_vocabulary_machine_name_load('vocab_project_status');
  $query = new EntityFieldQuery();
  $result = $query->entityCondition('entity_type', 'taxonomy_term')
  ->propertyCondition('vid', $status_vocab->vid)
  ->propertyCondition('name', array('Bad', 'Worst'), 'IN')
  ->propertyOrderBy('weight')
  ->execute();

  $status_tids = array();
  if (!empty($result)) {
    $status_tids = array_keys($result['taxonomy_term']);
  }

  if (!empty($status_tids)) {
    $bad_projects = array();
    foreach ($tids as $key => $tid) {
      $query = new EntityFieldQuery();
      $result = $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'ct_scrum')
      ->propertyCondition('status', NODE_PUBLISHED)
      ->fieldCondition('field_ct_scrum', 'tid', $tid)
      ->fieldCondition('field_ct_project_status', 'tid', $status_tids, 'IN')
      ->range(0, 1)
      ->propertyCondition('created', array($start, $end), "between")
      ->execute();

      if (!empty($result)) {
        $nid = array_keys($result['node']);
        $nodes[] = $nid[0];
        $bad_projects[$key]['tid'] = $tid;
        $bad_projects[$key]['nid'] = $nid[0];
      }
    }

    if (!empty($bad_projects)) {
      $mail_ids = array();
      $scrum_mail_detail = array();
      foreach ($bad_projects as $project) {
        $tid = $project['tid'];
        $nid = $project['nid'];

        $term = taxonomy_term_load($tid);
        $user_id = field_get_items('taxonomy_term', $term, 'field_ct_scrum_master');
        $scrum_mail_detail[$term->tid]['project_name'] = $term->name;

        if (isset($user_id[0]['target_id'])) {
          $uid = $user_id[0]['target_id'];
          $user = user_load($uid);
          $mail_ids[] = isset($user->mail) ? $user->mail : NULL;
          $scrum_mail_detail[$term->tid]['email'] = isset($user->mail) ? $user->mail : NULL;
          $scrum_mail_detail[$term->tid]['scrum_master'] = isset($user->name) ? $user->name : NULL;
        }

        $node = node_load($nid);
        $name = NULL;
        $summary = NULL;

        $project_status = field_get_items('node', $node, 'field_ct_project_status');
        if (!empty($project_status)) {
          $project_status = $project_status[0]['tid'];
          $project_status = taxonomy_term_load($project_status);
          $name = $project_status->name;
        }

        $summary_value = field_get_items('node', $node, 'body');
        if (!empty($summary_value)) {
          $summary = $summary_value[0]['safe_value'];
        }

        $scrum_mail_detail[$term->tid]['project_status'] = $name;
        $scrum_mail_detail[$term->tid]['summary'] = $summary;
        $scrum_mail_detail[$term->tid]['date'] = date('d-M-y', $node->created);
      }

      $scrum_template = theme('scrum_mail_template_bad_status', array('data' => $scrum_mail_detail));

      $mail_to = 'basant.sharma@innoraft.com';
      if (!is_null($mail_to)) {
        $from = variable_get('site_mail', '');
        $to = $mail_to;
        $ln = language_list();
        $from = $from;
        $language = $ln['en'];
        $params['subject'] = 'Project status';

        $params['body'][0] = $scrum_template;
        $mail_cc = variable_get('scrum_mail_cc', 'mukesh.agarwal@innoraft.com,basant.sharma@innoraft.com');
        $params['headers'] = array(
          'Cc' => $mail_cc,
          'MIME-Version' => '1.0',
          'Content-Type' => 'text/html; charset=UTF-8; format=flowed',
        );

        drupal_mail('scrum_general_apis', 'scrum_mail_key', $to, $language, $params, $from, TRUE);
      }
    }
  }
}
