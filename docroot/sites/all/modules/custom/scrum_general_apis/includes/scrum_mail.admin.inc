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
