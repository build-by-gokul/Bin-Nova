# Security Policy

## Reporting

Please report security vulnerabilities privately to the repository maintainer rather than publishing credentials or exploit details in an issue.

## Credential Safety

- Never commit secrets.h or real credentials.
- Use secrets.example.h only for placeholders.
- Rotate exposed credentials immediately.
- Verify TLS certificates in production deployments.
