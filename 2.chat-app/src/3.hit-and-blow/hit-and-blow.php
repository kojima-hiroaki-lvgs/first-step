#!/usr/bin/env php

<?php
/**
 * > ./hit-and-blow
 * コマンドライン引数なしで起動されたときは、対戦者の接続を待ちましょう
 *
 * > ./hit-and-blow 192.168.0.x
 * コマンドライン引数に対戦相手を指定して起動されたときは対戦相手に接続を試みるようにしましょう
 */


if ($argc > 1) {
    // コマンドライン引数ありで起動されたときの処理
    $opponent_address = $argv[1];
} else {
  // コマンドライン引数なしで起動されたときの処理
}