"""Validate and generate localization catalogs. Use --check to verify generated files."""

import argparse
import json
import re
import string
from pathlib import Path


def cpp_string(value: str) -> str:
    if any(ord(character) > 127 for character in value):
        return '"' + ''.join(f'\\x{byte:02x}' for byte in value.encode('utf-8')) + '"'
    return json.dumps(value, ensure_ascii=True)


PRINTF = re.compile(r"%(?:%|[-+ #0']*(?:\d+|\*)?(?:\.(?:\d+|\*))?(?:hh|ll|I64|I32|[hljztL])?[diuoxXfFeEgGaAcspn])")

# Backward-compatibility contract for IDs already persisted in user configs.
# languages.json remains the source of language metadata; new languages may be appended.
PERSISTED_LANGUAGE_IDS = ['English', 'SimplifiedChinese', 'TraditionalChinese', 'Japanese', 'Korean']


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f'Duplicate JSON key: {key}')
        result[key] = value
    return result


def read_json(path):
    return json.loads(path.read_text(encoding='utf-8'), object_pairs_hook=unique_object)


def format_fields(value):
    return [(field, spec, conversion) for _, field, spec, conversion in string.Formatter().parse(value)
            if field is not None]


def validate_catalog(catalog, reference, name):
    if catalog.keys() != reference.keys():
        missing = sorted(reference.keys() - catalog.keys())
        extra = sorted(catalog.keys() - reference.keys())
        raise ValueError(f'{name}: missing keys {missing}; extra keys {extra}')
    for source, translated in catalog.items():
        if not isinstance(translated, str) or not translated:
            raise ValueError(f'{name}: empty or invalid translation for {source!r}')
        if PRINTF.findall(source) != PRINTF.findall(translated):
            raise ValueError(f'{name}: printf placeholders changed for {source!r}')
        if format_fields(source) != format_fields(translated):
            raise ValueError(f'{name}: format fields changed for {source!r}')
        if source.count('\n') != translated.count('\n'):
            raise ValueError(f'{name}: newline count changed for {source!r}')
        if '\0' in translated or '##' in translated:
            raise ValueError(f'{name}: translation introduces an ImGui ID or null byte for {source!r}')


def catalog_header(catalog, filename, variable):
    lines = [
        '#pragma once',
        f'// Generated from {filename}; edit the JSON catalog, not this file.',
        '#include <string_view>',
        '#include <unordered_map>',
        'namespace reframework::localization {',
        f'inline const std::unordered_map<std::string_view, const char*> {variable}{{',
    ]
    lines += ['    {' + cpp_string(key) + ', ' + cpp_string(value) + '},'
              for key, value in sorted(catalog.items())]
    lines += ['};', '} // namespace reframework::localization', '']
    return '\n'.join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    directory = Path(__file__).resolve().parents[1] / 'src' / 'localization'
    registry = read_json(directory / 'languages.json')
    languages = registry['languages']
    ids = [language['id'] for language in languages]
    tags = [language['tag'] for language in languages]
    if len(set(ids)) != len(ids) or len(set(tags)) != len(tags):
        raise ValueError('Duplicate language ID or tag')
    if ids[:len(PERSISTED_LANGUAGE_IDS)] != PERSISTED_LANGUAGE_IDS:
        raise ValueError('Preserve persisted language IDs; append new languages after the existing five')
    if registry['default'] != 'English':
        raise ValueError('The default language must remain English')
    outputs = {}
    reference = None
    reference_variable = None
    includes = []
    rows = []
    for language in languages:
        filename = language['catalog']
        variable = 'nullptr'
        if filename is not None:
            catalog = read_json(directory / filename)
            variable = language['tag'].replace('-', '_').lower()
            if reference is None:
                reference, reference_variable = catalog, variable
            validate_catalog(catalog, reference, filename)
            header = language['tag'].replace('-', '_') + '.hpp'
            outputs[header] = catalog_header(catalog, filename, variable)
            includes.append(f'#include "{header}"')
            variable = '&' + variable
        rows.append('    LanguageInfo{' + ', '.join([
            cpp_string(language['tag']), cpp_string(language['name']), variable]) + '},')
    if reference is None:
        raise ValueError('At least one translation catalog is required')
    lines = [
        '#pragma once',
        '// Generated from languages.json and catalogs; edit JSON sources, not this file.',
        '#include <array>',
        *includes,
        'namespace reframework::localization {',
        '// Numeric values are persisted in REFrameworkConfig_UILanguage; append new languages.',
        'enum class Language : int {',
        *[f'    {identifier} = {index},' for index, identifier in enumerate(ids)],
        '};',
        f'inline constexpr auto default_language = Language::{registry["default"]};',
        'using Catalog = std::unordered_map<std::string_view, const char*>;',
        'struct LanguageInfo { const char* tag; const char* name; const Catalog* catalog; };',
        'inline const std::array languages{',
        *rows,
        '};',
        '// All catalogs share this key set; use it to keep English widget IDs stable too.',
        f'inline const auto& source_catalog = {reference_variable};',
        '} // namespace reframework::localization',
        '',
    ]
    outputs['catalogs.hpp'] = '\n'.join(lines)
    for filename, content in outputs.items():
        target = directory / filename
        if args.check:
            if not target.exists() or target.read_text(encoding='utf-8') != content:
                raise SystemExit(f'{filename} is out of date; run scripts/generate_localization.py')
        else:
            target.write_text(content, encoding='utf-8', newline='\n')
    print(f'{"Verified" if args.check else "Generated"} {len(outputs) - 1} catalogs, '
          f'{len(reference)} entries each, {len(languages)} selectable languages')


if __name__ == '__main__':
    main()
