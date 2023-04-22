#pragma once

namespace ehal
{
    /*
     * Javascript snippet which provides a way to popuplate a "select" element with WiFi access point IDs returned
     * by a network scan, without blocking page load. If any of the returned SSIDs match the SSID we've previously
     * saved in our preferences, then attempt to select it automatically.
     */
    const char* SCRIPT_CONFIGURATION_PAGE PROGMEM = R"(function inject_ssid_list() {
        disable_all_inputs();
        let select = document.getElementById('wifi_ssid');
        let option = document.createElement('option');
        select.innerHTML = '';
        option.value = '';
        option.textContent = 'Loading SSIDs...';
        select.disabled = true;
        select.appendChild(option);

        let xhttp = new XMLHttpRequest();
        xhttp.open('GET', 'query_ssid', true);
        xhttp.onload = function() {
            let select = document.getElementById('wifi_ssid');
            if (this.status == 200) {
                select.disabled = false;
                select.innerHTML = '';
                let jsonResponse = JSON.parse(this.responseText);
                for (let i = 0; i < jsonResponse.ssids.length; ++i) {
                    let option = document.createElement('option');
                    option.value = jsonResponse.ssids[i];
                    option.textContent = jsonResponse.ssids[i];
                    if (jsonResponse.ssids[i] == '{{wifi_ssid}}') {
                        option.selected = true;
                    }
                    select.appendChild(option);
                }
            } else {
                select.disabled = true;
            }

            enable_all_inputs();
        }
        xhttp.error = enable_all_inputs;
        xhttp.timeout = enable_all_inputs;
        xhttp.send();
    }

    function clear_config() {
        disable_all_inputs();
        window.location = '/clear_config';
    }

    function toggle_inputs(state) {
        document.getElementById('reload').disabled = state;
        document.getElementById('save').disabled = state;
        document.getElementById('reset').disabled = state;
        document.getElementById('update').disabled = state;
    }

    function enable_all_inputs() {
        toggle_inputs(false);
    }

    function disable_all_inputs() {
        toggle_inputs(true);
    }

    function trigger_update() {
        disable_all_inputs();
        document.getElementById('update_form').submit();
    }

    function initial_ssid_query() {
        let option = document.getElementById('pre-ssid');
        if (!option || !option.value || option.value != '{{wifi_ssid}}') {
            inject_ssid_list();
        }
    }

    window.addEventListener('load', initial_ssid_query);)";

    const char* SCRIPT_WAIT_REBOOT PROGMEM = R"(function check_alive() {
        let xhttp = new XMLHttpRequest();
        xhttp.open('GET', 'query_life', true);
        xhttp.timeout = 1000;
        xhttp.onload = function() {
            if (this.status == 200) {
                window.location = '/';
            } else {
                update_reboot_progress();
            }
        }
        xhttp.error = update_reboot_progress;
        xhttp.timeout = update_reboot_progress;
        xhttp.send();
    }

    function update_reboot_progress() {
         let rebootProgress = document.getElementById('reboot_progress');
        rebootProgress.textContent += '.';
        check_alive();
    }

    function trigger_life_check() {
        window.setTimeout(check_alive, 2000);
    }

    window.addEventListener('load', trigger_life_check))";

    const char* SCRIPT_UPDATE_DIAGNOSTIC_LOGS PROGMEM = R"(function update_diagnostic_logs() {
        let xhttp = new XMLHttpRequest();
        xhttp.open('GET', 'query_diagnostic_logs', true);
        xhttp.onload = function() {
            let element = document.getElementById('logs');
            if (this.status == 200) {
                let jsonResponse = JSON.parse(this.responseText);
                for (let i = 0; i < jsonResponse.messages.length; ++i) {
                    if (element.textContent.indexOf(jsonResponse.messages[i]) == -1) {
                        element.textContent += jsonResponse.messages[i] + '\n';
                    }
                }
            }
        }
        xhttp.send();
    }

    window.setInterval(update_diagnostic_logs, 5000);
    window.addEventListener('load', update_diagnostic_logs);)";

    const char* SCRIPT_REDIRECT = R"(window.location = '{{uri}}';
    )";
} // namespace ehal