# Possibilidade de API

Esta seção do repositório mostra uma possibilidade no desenvolvimento de uma API em Python para realização de ações de forma automatizada e pré-programada no processo de medição.

## Diretórios

### creating_package

No diretório `creating_package` você terá acesso a um exemplo de código desenvolvido numa arquitetura de pacote Python, com a execução sendo feita por um arquivo `main.py`.

### notebook

No diretório `notebook` você terá acesso a um exemplo de código desenvolvido em (jupyter) notebook, com a execução sendo feita por um conjunto de células. Esse foi o método mostrado no vídeo gravado pelo Igor.

## Como usar

Cada diretório tem seu respectivo arquivo `requirements.txt`. Esse arquivo fornece as dependências que os códigos têm em bibliotecas que não vem nativamente com o Python.

**1. Clone este repositório** (para ter acesso aos arquivos de código).

**2. Abra a sua IDE no diretório que desejar** (`creating_package` ou `notebook`).

**3. Crie um ambiente virtual Python**. Abra o terminal/cmd e digite:

No Windows:
```python
python -m venv venv
```
No Linux/macOS:
```python
python3 -m venv venv
```

**4. Ative o ambiente virtual recém criado:**

No Windows:
```python
venv/Scripts/activate
```

No Linux/macOS:
```python
source venv/bin/activate
```

**5. Baixe as bibliotecas no `requirements.txt`:**

No Windows:
```python
pip install -r requirements.txt
```

No Linux/macOS:
```python
pip3 install -r requirements.txt
```

**6. Dependendo do diretório...**
-  Se estiver no diretório `creating_package`:
```python
python main.py
```
- Se estiver no diretório `notebook`:
Selecione o ambiente virtual recém criado para executar o código em notebook `api_laepi.ipynb` e depois execute as células de forma sequêncial (na ordem).
