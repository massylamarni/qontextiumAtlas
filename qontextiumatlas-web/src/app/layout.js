import './globals.css';

export const metadata = {
  title: 'QATLAS — Quantum Contextuality Atlas',
  description: 'A searchable database of quantum configurations classified by contextuality degree.',
};

export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  );
}
