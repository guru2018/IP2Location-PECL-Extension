<?php
        /*Test DB access using cache mathod*/ 
         $handle = ip2location_open('IP-COUNTRY-SAMPLE.BIN');
        ip2location_open_mem($handle, IP2LOCATION_CACHE_MEMORY);
        echo ip2location_get_country_short($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_country_long($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_region($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_city($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_isp($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_latitude($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_longitude($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_domain($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_zipcode($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_timezone($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_netspeed($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_iddcode($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_areacode($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_weatherstationcode($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_weatherstationname($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_mcc($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_mnc($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_mobilebrand($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_elevation($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_get_usagetype($handle, "25.5.10.2");
        echo "\n";
        echo ip2location_close($handle);
?>

