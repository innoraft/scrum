(function($) {
  Drupal.behaviors.scrum = {
    attach: function(context, settings) {

      $('select[name="field_ct_scrum[und]"]').change(function() {
        var tid = $(this).val();
        $('#block-views-views-scrum-last-scrum select').val(tid);
        $('#block-views-views-scrum-last-scrum .form-submit').click();
      });

      $(window).load(function(){
        var option = $('.node-ct_scrum-form .field-name-field-ct-scrum select option').size();
        if(typeof option != 'undefined') {
          tid = $('.node-ct_scrum-form .field-name-field-ct-scrum select option').val();
          $('#block-views-views-scrum-last-scrum select').val(tid);
          $('#block-views-views-scrum-last-scrum .form-submit').click();
        }
      });
    }
  };
})(jQuery);
