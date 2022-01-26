/*
 * Copyright 2018 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

(function($) {

    'use strict';

    $(function() {
        $('[data-toggle="tooltip"]').tooltip();
        $('[data-toggle="popover"]').popover();

        $('.popover-dismiss').popover({
            trigger: 'focus'
        })
    });


    function bottomPos(element) {
        return element.offset().top + element.outerHeight();
    }

    // Bootstrap Fixed Header
    $(function() {
        var promo = $(".js-td-cover");
        if (!promo.length) {
            return
        }

        var promoOffset = bottomPos(promo);
        var navbarOffset = $('.js-navbar-scroll').offset().top;

        var threshold = Math.ceil($('.js-navbar-scroll').outerHeight());
        if ((promoOffset - navbarOffset) < threshold) {
            $('.js-navbar-scroll').addClass('navbar-bg-onscroll');
        }


        $(window).on('scroll', function() {
            var navtop = $('.js-navbar-scroll').offset().top - $(window).scrollTop();
            var promoOffset = bottomPos($('.js-td-cover'));
            var navbarOffset = $('.js-navbar-scroll').offset().top;
            if ((promoOffset - navbarOffset) < threshold) {
                $('.js-navbar-scroll').addClass('navbar-bg-onscroll');
            } else {
                $('.js-navbar-scroll').removeClass('navbar-bg-onscroll');
                $('.js-navbar-scroll').addClass('navbar-bg-onscroll--fade');
            }
        });
    });


}(jQuery));

;
/*
 * Copyright 2018 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

(function ($) {
    'use strict';

    // Headers' anchor link that shows on hover
    $(function () {
        // append anchor links to headings in markdown.
        var article = document.getElementsByTagName('main')[0];
        if (!article) {
            return;
        }
        var headings = article.querySelectorAll('h1, h2, h3, h4, h5, h6');
        headings.forEach(function (heading) {
            if (heading.id) {
                var a = document.createElement('a');
                // set visibility: hidden, not display: none to avoid layout change
                a.style.visibility = 'hidden';
                // [a11y] hide this from screen readers, etc..
                a.setAttribute('aria-hidden', 'true');
                // material insert_link icon in svg format
                a.innerHTML = ' <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" width="24" height="24" viewBox="0 0 24 24"><path d="M0 0h24v24H0z" fill="none"/><path d="M3.9 12c0-1.71 1.39-3.1 3.1-3.1h4V7H7c-2.76 0-5 2.24-5 5s2.24 5 5 5h4v-1.9H7c-1.71 0-3.1-1.39-3.1-3.1zM8 13h8v-2H8v2zm9-6h-4v1.9h4c1.71 0 3.1 1.39 3.1 3.1s-1.39 3.1-3.1 3.1h-4V17h4c2.76 0 5-2.24 5-5s-2.24-5-5-5z"/></svg>';
                a.href = '#' + heading.id;
                heading.insertAdjacentElement('beforeend', a);
                heading.addEventListener('mouseenter', function () {
                    a.style.visibility = 'initial';
                });
                heading.addEventListener('mouseleave', function () {
                    a.style.visibility = 'hidden';
                });
            }
        });
    });

}(jQuery));

;
/*
Copyright 2018 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

(function($) {

    'use strict';

    var Search = {
        init: function() {
            $(document).ready(function() {
               $(document).on('keypress', '.td-search-input', function(e) {
                    if (e.keyCode !== 13) {
                        return
                    }

                    var query = $(this).val();
                    var searchPage = "/lluvia/search/?q=" + query;
                    document.location = searchPage;

                    return false;
                });

            });
        },
    };

    Search.init();


}(jQuery));

;


(function($) {
    var needMermaid = false;
    $('.language-mermaid').parent().replaceWith(function() {
        needMermaid = true;
        return $('<pre class="mermaid">').text($(this).text());
    });

    if (!needMermaid)  {
        mermaid.initialize({startOnLoad: false});
        return;
    }

    var params = {"enable":true,"flowchart":{"diagrampadding":6},"theme":"neutral"};

    // site params are stored with lowercase keys; lookup correct casing
    // from Mermaid default config.
    var norm = function(defaultConfig, params) {
        var result = {};
        for (const key in defaultConfig) {
            const keyLower = key.toLowerCase();
            if (defaultConfig.hasOwnProperty(key) && params.hasOwnProperty(keyLower)) {
                if (typeof defaultConfig[key] === "object") {
                    result[key] = norm(defaultConfig[key], params[keyLower]);
                } else {
                    result[key] = params[keyLower];
                }
            }
        }
        return result;
    };
    var settings = norm(mermaid.mermaidAPI.defaultConfig, params);
    settings.startOnLoad = true;
    mermaid.initialize(settings);
})(jQuery);



;


;


;


(function () {
  var shade;
  var iframe;

  var insertFrame = function () {
    shade = document.createElement('div');
    shade.classList.add('drawioframe');
    iframe = document.createElement('iframe');
    shade.appendChild(iframe);
    document.body.appendChild(shade);
  }

  var closeFrame = function () {
    if (shade) {
      document.body.removeChild(shade);
      shade = undefined;
      iframe = undefined;
    }
  }

  var imghandler = function (img, imgdata) {
    var url = "https://embed.diagrams.net/";
    url += '?embed=1&ui=atlas&spin=1&modified=unsavedChanges&proto=json&saveAndEdit=1&noSaveBtn=1';

    var wrapper = document.createElement('div');
    wrapper.classList.add('drawio');
    img.parentNode.insertBefore(wrapper, img);
    wrapper.appendChild(img);

    var btn = document.createElement('button');
    btn.classList.add('drawiobtn');
    btn.insertAdjacentHTML('beforeend', '<i class="fas fa-edit"></i>');
    wrapper.appendChild(btn);

    btn.addEventListener('click', function (evt) {
      if (iframe) return;
      insertFrame();
      var handler = function (evt) {
        var wind = iframe.contentWindow;
        if (evt.data.length > 0 && evt.source == wind) {
          var msg = JSON.parse(evt.data);

          if (msg.event == 'init') {
            wind.postMessage(JSON.stringify({action: 'load', xml: imgdata}), '*');

          } else if (msg.event == 'save') {
            var fmt = imgdata.indexOf('data:image/png') == 0 ? 'xmlpng' : 'xmlsvg';
            wind.postMessage(JSON.stringify({action: 'export', format: fmt}), '*');

          } else if (msg.event == 'export') {
            const fn = img.src.replace(/^.*?([^/]+)$/, '$1');
            const dl = document.createElement('a');
            dl.setAttribute('href', msg.data);
            dl.setAttribute('download', fn);
            document.body.appendChild(dl);
            dl.click();
            dl.parentNode.removeChild(dl);
          }

          if (msg.event == 'exit' || msg.event == 'export') {
            window.removeEventListener('message', handler);
            closeFrame();
          }
        }
      };

      window.addEventListener('message', handler);
      iframe.setAttribute('src', url);
    });
  };


document.addEventListener('DOMContentLoaded', function () {
  // find all the png and svg images that may have embedded xml diagrams
  for (const el of document.getElementsByTagName('img')) {
    const img = el;
    const src = img.getAttribute('src');
    if (!src.endsWith('.svg') && !src.endsWith('.png')) {
      continue;
    }

    const xhr = new XMLHttpRequest();
    xhr.responseType = 'blob';
    xhr.open("GET", src);
    xhr.addEventListener("load", function () {
      const fr = new FileReader();
      fr.addEventListener('load', function () {
        if (fr.result.indexOf('mxfile') != -1) {
          const fr = new FileReader();
          fr.addEventListener('load', function () {
            const imgdata = fr.result;
            imghandler(img, imgdata);
          });
          fr.readAsDataURL(xhr.response);
        }
      });
      fr.readAsBinaryString(xhr.response);
    });
    xhr.send();
  };
});
}());


