#!/usr/bin/env python

import argparse
import os
import re

def main():
    args = parse_args()
    config = load_config(args.config)
    config['SENSOR_NAME'] = args.sensor_name

    with open(args.script, 'r') as f:
        script = f.read()

    updated_script = replace_placeholders(script, config)

    target_path = os.path.join('target', args.script)
    os.makedirs(os.path.dirname(target_path), exist_ok=True)
    with open(target_path, 'w+') as f:
        f.write(updated_script)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('script', help='Script to apply configuration to')
    parser.add_argument('sensor_name')
    parser.add_argument('config', help='Configuration file')
    return parser.parse_args()


def load_config(file):
    with open(file, 'r') as f:
        lines = f.readlines()

    conf = {}
    for line in lines:
        if '=' in line:
            k, v = line.split('=')
            conf[k] = v.strip()

    return conf


def replace_placeholders(text, values):
    updated_text = ''
    prev_idx = 0

    for m in re.finditer('\$\{([A-Z_]+)\}', text):
        placeholder = m.group(1)

        updated_text += text[prev_idx:m.start()]
        updated_text += values.get(placeholder, 'VALUE_MISSING')
        prev_idx = m.end()

    updated_text += text[prev_idx:]
    return updated_text


if __name__ == "__main__":
    main()
